// Sync harness for the sound nodes: measures, in samples, how far each
// timestretch mode strays from the ideal output grid, for files whose native
// tempo differs from the timeline tempo, both when started from the beginning
// and when dropped in mid-playback.
//
// The signal is a click track: one full-scale sample every 60/file_tempo
// seconds, silence elsewhere. Sync error then *is* impulse position error:
// for each output we locate every click and compare it against the position
// it must occupy, so two files agreeing with each other while both drifting
// still fail, which a pairwise cross-correlation would miss.
//
// The token stream mirrors what score actually produces (buffer_tick ->
// scenario -> time_interval::tick_offset): for a transport tempo T and
// transport speed s,
//   - token.tempo  = s * T                  (time_interval::m_current_tempo)
//   - token.speed  = s * T / root_tempo     (time_interval::m_globalSpeed)
//   - the model date advances by floor(buffer_flicks * speed + residue) with
//     the residue carried, exactly like time_interval::take_step().
// The earlier harness in SoundTest.cpp advanced the model date at unit rate
// with token.speed = 1, which only coincides with production when T == 120;
// every conclusion drawn at other tempi through it was measured against a
// token stream the engine never emits.
//
// Under this convention the file position of a stretched sound is a pure
// function of the model date: consumption is tempo(t)/file_tempo file samples
// per physical sample while the model advances at tempo(t)/root_tempo, so
// d(file)/d(model) = root_tempo / file_tempo whatever the tempo curve or
// speed do. The drop-in tests pin exactly that invariant.
//
// The file is deliberately self-contained over the public-ish node API so it
// can be cherry-picked onto older revisions: the carried sample span fields
// of token_request are detected at compile time and skipped when absent.

#define _USE_MATH_DEFINES
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/nodes/sound_ref.hpp>

#include "include_catch.hpp"

#include <ossia/detail/thread.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <vector>

namespace
{
using namespace ossia;

constexpr int64_t fps = ossia::flicks_per_second<int64_t>;

template <typename T>
concept has_sample_span = requires(T t) {
  t.start_sample;
  t.length_sample;
};

// Reproduces the token stream time_interval emits when driven by buffer_tick
// with a transport tempo and speed: floor-with-residue model advancement, the
// global speed in token.speed, the tempo scaled by the speed in token.tempo.
struct production_ticker
{
  int sampleRate{};
  int bufferSize{};
  double transport_tempo{};
  double speed{};

  int64_t model_date = 0;
  double residue = 0.;

  [[nodiscard]] double global_speed() const noexcept
  {
    return speed * transport_tempo / ossia::root_tempo;
  }

  ossia::token_request next() noexcept
  {
    const int64_t buf_flicks = int64_t(bufferSize) * (fps / sampleRate);
    const double want = double(buf_flicks) * global_speed() + residue;
    const double step = std::floor(want);
    residue = want - step;
    const int64_t adv = int64_t(step);

    ossia::token_request tk{
        ossia::time_value{model_date},
        ossia::time_value{model_date + adv},
        ossia::time_value{fps * int64_t(3600)},
        ossia::time_value{0},
        global_speed(),
        {4, 4},
        speed * transport_tempo};
    // Dependent context so the branch is genuinely discarded on revisions
    // whose token_request has no carried sample span.
    [&]<typename TK>(TK& t) {
      if constexpr(has_sample_span<TK>)
      {
        t.start_sample = 0;
        t.length_sample = int32_t(bufferSize);
      }
    }(tk);
    model_date += adv;
    return tk;
  }
};

// One sample of 1.0 every 60/tempo seconds. Click k sits at round(k*period),
// so its exact file position is known and the expected output position can be
// derived from it rather than from the idealized (fractional) grid.
ossia::audio_array make_click_track(double file_tempo, int sampleRate, int64_t frames)
{
  ossia::audio_array d;
  d.resize(1);
  d[0].assign(frames, 0.f);
  const double period = 60.0 / file_tempo * sampleRate;
  for(int64_t k = 0;; k++)
  {
    const auto pos = int64_t(std::llround(double(k) * period));
    if(pos >= frames)
      break;
    d[0][pos] = 1.f;
  }
  return d;
}

std::unique_ptr<ossia::nodes::sound_ref> make_click_node(
    const ossia::audio_array& data, ossia::audio_stretch_mode mode, int sampleRate,
    double file_tempo)
{
  auto n = std::make_unique<ossia::nodes::sound_ref>();
  n->set_sound(data);
  // The testing set_sound() forces stretch mode None; override it.
  n->m_resampler.reset(0, mode, 1, sampleRate);
  n->set_native_tempo(file_tempo);
  return n;
}

// Runs the node over tokens [first_tick, first_tick + n_ticks[ of the shared
// timeline and returns its output, one bufferSize block per tick. The port is
// cleared before every tick so unwritten samples read as silence instead of
// stale data.
std::vector<float> run_node(
    ossia::nodes::sound_ref& n, ossia::execution_state& e,
    const std::vector<ossia::token_request>& tokens, std::size_t first_tick,
    std::size_t n_ticks, int bufferSize)
{
  std::vector<float> out;
  out.reserve(n_ticks * bufferSize);
  for(std::size_t k = first_tick; k < first_tick + n_ticks; k++)
  {
    n.audio_out.data.get().clear();
    n.run(tokens[k], ossia::exec_state_facade{&e});
    if(n.audio_out.data.channels() >= 1)
    {
      auto& ch = n.audio_out.data.channel(0);
      for(int i = 0; i < bufferSize; i++)
        out.push_back(i < int(ch.size()) ? float(ch[i]) : 0.f);
    }
    else
    {
      out.insert(out.end(), bufferSize, 0.f);
    }
  }
  return out;
}

struct click_stats
{
  int found{};
  int missing{};
  double median_err{};  // signed samples, found - expected
  double max_abs_err{};
  double first_err{};
  double last_err{};
  double drift_per_min{}; // least-squares slope of error vs position, samples/min
};

// For each expected click position, search +-radius for the peak; a window
// whose maximum stays below the threshold counts as a missing click. This is
// robust to the smearing of a stretcher as long as the peak stays the peak.
click_stats measure_clicks(
    const std::vector<float>& out, const std::vector<double>& expected, double radius,
    int sampleRate, float threshold = 0.25f)
{
  click_stats st{};
  std::vector<double> errs;
  std::vector<double> poss;
  for(double pos : expected)
  {
    const auto lo = int64_t(std::floor(pos - radius));
    const auto hi = int64_t(std::ceil(pos + radius));
    if(lo < 0 || hi >= int64_t(out.size()))
      continue;
    int64_t best = lo;
    float best_v = 0.f;
    for(int64_t i = lo; i <= hi; i++)
    {
      const float v = std::abs(out[i]);
      if(v > best_v)
      {
        best_v = v;
        best = i;
      }
    }
    if(best_v < threshold)
    {
      st.missing++;
      continue;
    }
    errs.push_back(double(best) - pos);
    poss.push_back(pos);
  }
  st.found = int(errs.size());
  if(errs.empty())
    return st;

  st.first_err = errs.front();
  st.last_err = errs.back();
  for(double e : errs)
    st.max_abs_err = std::max(st.max_abs_err, std::abs(e));

  auto sorted = errs;
  std::sort(sorted.begin(), sorted.end());
  st.median_err = sorted[sorted.size() / 2];

  // Least-squares slope of err against position, in samples per minute.
  if(errs.size() >= 2)
  {
    double mx = 0, my = 0;
    for(std::size_t i = 0; i < errs.size(); i++)
    {
      mx += poss[i];
      my += errs[i];
    }
    mx /= errs.size();
    my /= errs.size();
    double num = 0, den = 0;
    for(std::size_t i = 0; i < errs.size(); i++)
    {
      num += (poss[i] - mx) * (errs[i] - my);
      den += (poss[i] - mx) * (poss[i] - mx);
    }
    if(den > 0)
      st.drift_per_min = num / den * 60. * sampleRate;
  }
  return st;
}

const char* mode_name(ossia::audio_stretch_mode m)
{
  switch(m)
  {
    case ossia::audio_stretch_mode::None:
      return "raw     ";
    case ossia::audio_stretch_mode::Repitch:
      return "repitch ";
    case ossia::audio_stretch_mode::RubberBandStandard:
      return "rubber  ";
    default:
      return "?       ";
  }
}

struct sync_case
{
  ossia::audio_stretch_mode mode{};
  double file_tempo{};
  double transport_tempo{};
  double speed{1.0};
  int bufferSize{512};
};

struct sync_result
{
  sync_case c;
  click_stats started; // node playing from 0, measured after the drop point
  click_stats dropped; // node dropped at the drop point
  int64_t seek_actual{};
  int64_t seek_expected{};
};

// Runs one scenario: node A plays from 0; node B is dropped mid-playback the
// way add_time_process() drops it (transport(m_date, current_transport_info())).
// Both outputs are measured against the absolute expected click positions.
sync_result run_sync_case(const sync_case& c)
{
  constexpr int sampleRate = 44100;
  const int N = c.bufferSize;

  const double phys_seconds = 12.0;
  const std::size_t n_ticks = std::size_t(phys_seconds * sampleRate) / N;
  const std::size_t drop_tick = n_ticks / 3;
  const int64_t P0 = int64_t(drop_tick) * N; // physical sample of the drop

  // File consumption rate in file samples per physical output sample.
  const bool stretching = c.mode != ossia::audio_stretch_mode::None;
  const double consumption
      = stretching ? c.speed * c.transport_tempo / c.file_tempo : 1.0;

  // Enough file for the whole run at the fastest consumption, plus slack.
  const auto file_frames
      = int64_t(phys_seconds * sampleRate * consumption) + 8 * 65536;
  const auto data = make_click_track(c.file_tempo, sampleRate, file_frames);

  ossia::execution_state e;
  e.bufferSize = N;
  e.sampleRate = sampleRate;
  e.modelToSamplesRatio = double(sampleRate) / double(fps);
  e.samplesToModelRatio = double(fps) / double(sampleRate);

  production_ticker ticker{sampleRate, N, c.transport_tempo, c.speed};
  std::vector<ossia::token_request> tokens;
  tokens.reserve(n_ticks);
  for(std::size_t k = 0; k < n_ticks; k++)
    tokens.push_back(ticker.next());

  auto A = make_click_node(data, c.mode, sampleRate, c.file_tempo);
  const auto outA = run_node(*A, e, tokens, 0, n_ticks, N);

  auto B = make_click_node(data, c.mode, sampleRate, c.file_tempo);
  ossia::tick_transport_info tinfo{};
  tinfo.date = tokens[drop_tick].prev_date;
  tinfo.current_tempo = c.speed * c.transport_tempo;
  B->transport(tokens[drop_tick].prev_date, tinfo);

  sync_result r{};
  r.c = c;
  r.seek_actual = B->m_resampler.next_sample_to_read();
  r.seek_expected = int64_t(std::llround(double(P0) * consumption));

  const auto outB = run_node(*B, e, tokens, drop_tick, n_ticks - drop_tick, N);

  // Expected positions: click k sits at file position F_k; it must come out
  // at physical F_k / consumption. For B the origin is the drop point.
  const double period = 60.0 / c.file_tempo * sampleRate;
  const double out_spacing = period / consumption;
  const double radius = std::min(0.45 * out_spacing, 4096.);

  std::vector<double> expectA, expectB;
  for(int64_t k = 0;; k++)
  {
    const double F = double(int64_t(std::llround(double(k) * period)));
    const double P = F / consumption;
    if(P >= double(n_ticks * N))
      break;
    // Skip the transient right at the start of each stream: the first click
    // of a freshly primed stretcher is starting-latency, not sync.
    if(P > 2.5 * out_spacing)
      expectA.push_back(P);
    if(P - double(P0) > 2.5 * out_spacing)
      expectB.push_back(P - double(P0));
  }

  // Measure A over the same span B exists in, so the two stats face the same
  // part of the timeline.
  std::vector<double> expectA_tail;
  for(double p : expectA)
    if(p > double(P0))
      expectA_tail.push_back(p);

  r.started = measure_clicks(outA, expectA_tail, radius, sampleRate);
  r.dropped = measure_clicks(outB, expectB, radius, sampleRate);
  return r;
}

void print_result_header()
{
  std::fprintf(
      stderr,
      "\n%-9s %5s %5s %6s %5s | %7s %8s %9s %4s | %7s %8s %9s %4s | %10s\n",
      "mode", "fTmp", "tTmp", "speed", "buf", "st.med", "st.max", "st.drift",
      "miss", "dr.med", "dr.max", "dr.drift", "miss", "seek err");
}

void print_result(const sync_result& r)
{
  std::fprintf(
      stderr,
      "%-9s %5.0f %5.0f %6.3f %5d | %7.1f %8.1f %9.3f %4d | %7.1f %8.1f %9.3f %4d | %10lld\n",
      mode_name(r.c.mode), r.c.file_tempo, r.c.transport_tempo, r.c.speed,
      r.c.bufferSize, r.started.median_err, r.started.max_abs_err,
      r.started.drift_per_min, r.started.missing, r.dropped.median_err,
      r.dropped.max_abs_err, r.dropped.drift_per_min, r.dropped.missing,
      (long long)(r.seek_actual - r.seek_expected));
}
}

#if defined(OSSIA_ENABLE_RUBBERBAND) && defined(OSSIA_ENABLE_LIBSAMPLERATE)

TEST_CASE("sound_sync_sweep", "[sound][sync]")
{
  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);

  // Experimentation knob: OSSIA_SOUND_SYNC_PRIME=<0..4> selects the
  // rubberband priming strategy for the whole sweep. Unset = production
  // default.
  if(const char* p = std::getenv("OSSIA_SOUND_SYNC_PRIME"))
  {
    ossia::rubberband_stretcher::s_prime_strategy
        = ossia::rubberband_stretcher::prime_strategy(std::atoi(p));
    std::fprintf(stderr, "prime strategy override: %s\n", p);
  }

  using m = ossia::audio_stretch_mode;
  std::vector<sync_case> cases;

  // Baseline: buffer 512, speed 1.
  for(double fT : {90., 120., 128., 140.})
    for(double tT : {120., 140.})
      for(auto mode : {m::None, m::Repitch, m::RubberBandStandard})
        cases.push_back({mode, fT, tT, 1.0, 512});

  // Small buffers.
  for(double fT : {90., 128.})
    for(auto mode : {m::None, m::Repitch, m::RubberBandStandard})
      cases.push_back({mode, fT, 140., 1.0, 64});

  // Fractional transport speed: buffer_frames * speed is not a whole number
  // of flicks, the residue-carry path is active every tick.
  for(double fT : {90., 128.})
    for(double tT : {120., 140.})
      for(auto mode : {m::None, m::Repitch, m::RubberBandStandard})
        cases.push_back({mode, fT, tT, 1.234, 512});
  for(auto mode : {m::None, m::Repitch, m::RubberBandStandard})
    cases.push_back({mode, 128., 140., 1.234, 64});

  print_result_header();
  for(const auto& c : cases)
  {
    const auto r = run_sync_case(c);
    print_result(r);

    INFO(
        mode_name(r.c.mode) << " fT=" << r.c.file_tempo << " tT="
                            << r.c.transport_tempo << " s=" << r.c.speed
                            << " buf=" << r.c.bufferSize);

    const bool stretching = c.mode != m::None;
    if(stretching)
    {
      // Files started together must sit on the grid; files dropped in must
      // land on the same grid. Repitch is sample-accurate; the R2 rubberband
      // engine places transients with a bounded jitter of up to ~2 hops
      // around the correct position, so its tolerances are its noise floor,
      // not an accepted drift. Drift itself is asserted in the long-run test
      // below: over these 8-second windows a least-squares slope through
      // R2's jitter measures the jitter, not the tracking.
      const double med_tol = c.mode == m::RubberBandStandard ? 48. : 2.;
      const double max_tol = c.mode == m::RubberBandStandard ? 160. : 4.;
      CHECK(r.started.missing == 0);
      CHECK(std::abs(r.started.median_err) < med_tol);
      CHECK(r.started.max_abs_err < max_tol);

      CHECK(r.dropped.missing == 0);
      CHECK(std::abs(r.dropped.median_err) < med_tol);
      CHECK(r.dropped.max_abs_err < max_tol);

      // And the dropped file must agree with the running one.
      const double agree_tol = c.mode == m::RubberBandStandard ? 32. : 2.;
      CHECK(std::abs(r.dropped.median_err - r.started.median_err) < agree_tol);
    }

    // A dropped file must continue from the file position the running copy
    // has reached - in every mode, including raw, where the file position is
    // simply the physical time elapsed.
    CHECK(std::llabs(r.seek_actual - r.seek_expected) <= 1);
  }
}

// Files started together and left running for a whole minute: quantifies
// long-run drift much more sensitively than the sweep, for the two stretchers
// at an irrational tempo ratio.
TEST_CASE("sound_sync_long_run", "[sound][sync]")
{
  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);

  constexpr int sampleRate = 44100;
  constexpr int N = 512;
  const double phys_seconds = 60.0;
  const std::size_t n_ticks = std::size_t(phys_seconds * sampleRate) / N;

  struct
  {
    double fT, tT;
  } ratios[] = {{90., 140.}, {128., 140.}, {140., 120.}};

  for(const auto& [fT, tT] : ratios)
  {
    const double consumption = tT / fT;

    const auto file_frames
        = int64_t(phys_seconds * sampleRate * consumption) + 8 * 65536;
    const auto data = make_click_track(fT, sampleRate, file_frames);

    ossia::execution_state e;
    e.bufferSize = N;
    e.sampleRate = sampleRate;
    e.modelToSamplesRatio = double(sampleRate) / double(fps);
    e.samplesToModelRatio = double(fps) / double(sampleRate);

    production_ticker ticker{sampleRate, N, tT, 1.0};
    std::vector<ossia::token_request> tokens;
    for(std::size_t k = 0; k < n_ticks; k++)
      tokens.push_back(ticker.next());

    const double period = 60.0 / fT * sampleRate;
    const double out_spacing = period / consumption;

    for(auto mode :
        {ossia::audio_stretch_mode::Repitch,
         ossia::audio_stretch_mode::RubberBandStandard})
    {
      auto n = make_click_node(data, mode, sampleRate, fT);
      const auto out = run_node(*n, e, tokens, 0, n_ticks, N);

      std::vector<double> expect;
      for(int64_t k = 0;; k++)
      {
        const double F = double(int64_t(std::llround(double(k) * period)));
        const double P = F / consumption;
        if(P >= double(n_ticks * N))
          break;
        if(P > 2.5 * out_spacing)
          expect.push_back(P);
      }

      const auto st = measure_clicks(
          out, expect, std::min(0.45 * out_spacing, 4096.), sampleRate);
      std::fprintf(
          stderr,
          "[long run %s %3.0f->%3.0f] clicks=%d missing=%d median=%.1f max=%.1f "
          "first=%.1f last=%.1f drift=%.3f samples/min\n",
          mode_name(mode), fT, tT, st.found, st.missing, st.median_err,
          st.max_abs_err, st.first_err, st.last_err, st.drift_per_min);

      INFO(mode_name(mode) << " " << fT << "->" << tT);
      const bool rb = mode == ossia::audio_stretch_mode::RubberBandStandard;
      CHECK(st.missing == 0);
      // Real long-run drift: repitch tracks the ratio exactly; R2's slope
      // estimate carries its transient jitter divided by the window, hence
      // the wider bound.
      CHECK(std::abs(st.drift_per_min) < (rb ? 8.0 : 1.0));
      CHECK(std::abs(st.median_err) < (rb ? 48. : 2.));
    }
  }
}

// The invariant the drop-in seek must respect: with production tokens the
// file position of a stretched sound is model_samples * root_tempo/file_tempo
// no matter the transport tempo or speed, because both the model clock and
// the consumption scale with the live tempo. A dropped file that seeks
// anywhere else starts flamming against the copies already playing.
TEST_CASE("sound_sync_drop_seek_invariant", "[sound][sync]")
{
  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);

  using m = ossia::audio_stretch_mode;
  struct
  {
    m mode;
    double fT, tT, speed;
  } cases[] = {
      {m::Repitch, 100., 120., 1.0},  // the historical baseline: must not move
      {m::Repitch, 90., 140., 1.0},
      {m::Repitch, 128., 140., 1.234},
      {m::RubberBandStandard, 90., 140., 1.0},
      {m::RubberBandStandard, 128., 140., 1.234},
      {m::None, 128., 140., 1.0},
  };

  constexpr int sampleRate = 44100;
  constexpr int N = 512;
  for(const auto& c : cases)
  {
    const bool stretching = c.mode != m::None;
    const double consumption = stretching ? c.speed * c.tT / c.fT : 1.0;

    // 4 seconds in, like the sweep's drop point.
    production_ticker ticker{sampleRate, N, c.tT, c.speed};
    const std::size_t drop_tick = std::size_t(4.0 * sampleRate) / N;
    ossia::token_request last{};
    for(std::size_t k = 0; k < drop_tick; k++)
      last = ticker.next();
    const int64_t P0 = int64_t(drop_tick) * N;

    ossia::audio_array data;
    data.resize(1);
    data[0].assign(1024, 0.f);
    auto node = make_click_node(data, c.mode, sampleRate, c.fT);

    ossia::tick_transport_info tinfo{};
    tinfo.date = last.date;
    tinfo.current_tempo = c.speed * c.tT;
    node->transport(last.date, tinfo);

    const auto seek = node->m_resampler.next_sample_to_read();
    const auto want = int64_t(std::llround(double(P0) * consumption));
    INFO(
        mode_name(c.mode) << " fT=" << c.fT << " tT=" << c.tT << " s=" << c.speed
                          << " seek=" << seek << " want=" << want);
    CHECK(std::llabs(seek - want) <= 1);
  }
}

#endif
