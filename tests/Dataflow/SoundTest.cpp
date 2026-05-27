
#define _USE_MATH_DEFINES
#define DR_WAV_IMPLEMENTATION 1
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/nodes/sound_mmap.hpp>
#include <ossia/dataflow/nodes/sound_ref.hpp>

#include "include_catch.hpp"

#include <ossia/detail/thread.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <numeric>

TEST_CASE("test_sound_ref", "test_sound_ref")
{
  using namespace ossia;
  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);
  nodes::sound_ref snd;
  snd.set_sound(ossia::audio_array{{0.1, 0.2, 0.3, 0.4}});

  execution_state e;
  e.bufferSize = 9;

  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 0_tv, .offset = 0_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 4_tv, .offset = 0_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 0_tv, .offset = 4_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 4_tv, .offset = 4_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 0_tv, .offset = 8_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 1_tv, .offset = 8_tv});

  for(auto tk : snd.requested_tokens)
  {
    snd.run(tk, {&e});
  }

  auto op = snd.root_outputs()[0]->target<audio_port>()->get();
  audio_vector expected{
      audio_channel{0.1f, 0.2f, 0.3f, 0.4f, 0.1f, 0.2f, 0.3f, 0.4f, 0.1f}};
  for(int i = 0; i < 9; i++)
  {
    REQUIRE(expected[0][i] - op[0][i] < 0.00001);
  }
  REQUIRE(op == expected);
}

#if defined(__GNUC__) || defined(__clang__)
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
// https://gist.github.com/Jon-Schneider/8b7c53d27a7a13346a643dac9c19d34f
struct __attribute__((packed, aligned(8))) test_wave
{
  static constexpr int nc = 1;
  static constexpr int F = 44100;
  static constexpr int ns = 1;
  static constexpr int M = 4;

  // RIFF Header
  char ckID[4] = {'R', 'I', 'F', 'F'};
  int cksize = 4 + 48 + 12 + (8 + M * nc * ns + 0);
  char WAVEID[4] = {'W', 'A', 'V', 'E'};

  // Format Header
  char fmt[4] = {'f', 'm', 't', ' '};
  int fmt_cksize = 18;
  short wFormatTag = 3;
  short nChannels = nc;
  int nSamplesPerSec = F;
  int nAvgBytesPerSec = F * M * nc;
  short nBlockAlign = M * nc;
  short wBitsPerSample = 8 * M;
  short cbSize = 0;

  // Facts
  char factID[4] = {'f', 'a', 'c', 't'};
  int factSize = 4;
  int dwSampleLength = nc * ns;

  // Data
  char data_header[4] = {'d', 'a', 't', 'a'};
  int data_bytes = 4 * 1 * 32;

  float v[M * nc * ns] = {0.1, 0.2, 0.3, 0.4};
};

TEST_CASE("test_sound_mmap", "test_sound_mmap")
{
  using namespace ossia;
  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);
  nodes::sound_mmap snd;

  test_wave w;
  drwav_handle h{&w, sizeof(test_wave)};
  float v[4]{};
  h.read_pcm_frames_f32(4, v);
  REQUIRE(v[0] == 0.1f);
  REQUIRE(v[1] == 0.2f);
  REQUIRE(v[2] == 0.3f);
  REQUIRE(v[3] == 0.4f);

  snd.set_sound(h);

  execution_state e;
  e.bufferSize = 9;

  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 0_tv, .offset = 0_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 4_tv, .offset = 0_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 0_tv, .offset = 4_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 4_tv, .offset = 4_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 0_tv, .offset = 8_tv});
  snd.requested_tokens.push_back(
      simple_token_request{.prev_date = 0_tv, .date = 1_tv, .offset = 8_tv});

  for(auto tk : snd.requested_tokens)
  {
    snd.run(tk, {&e});
    auto op = snd.root_outputs()[0]->target<audio_port>()->get();
  }

  auto op = snd.root_outputs()[0]->target<audio_port>()->get();
  audio_vector expected{
      audio_channel{0.1f, 0.2f, 0.3f, 0.4f, 0.1f, 0.2f, 0.3f, 0.4f, 0.1f}};
  for(int i = 0; i < 9; i++)
  {
    REQUIRE(expected[0][i] - op[0][i] < 0.00001);
  }
  REQUIRE(op == expected);
}
#endif

#if defined(OSSIA_ENABLE_RUBBERBAND)
// Sweep test: measures alignment between a sound dropped mid-playback and one
// that has been playing from the start. Reports xcorr via stderr; not a
// CI-style PASS/FAIL.
namespace
{
struct sync_measurement
{
  int best_lag_samples{};
  double best_correlation{};
  double rms_expected{};
  double rms_dropped{};
  double rms_difference_at_best_lag{};
};

// Normalized cross-correlation; positive lag = dropped trails expected.
inline sync_measurement
measure_sync(
    const std::vector<float>& expected, const std::vector<float>& dropped,
    int window_start, int window_len, int max_lag)
{
  sync_measurement m{};

  auto rms = [](const float* p, int n) {
    long double s = 0;
    for(int i = 0; i < n; i++)
      s += (long double)p[i] * (long double)p[i];
    return std::sqrt(double(s / std::max(1, n)));
  };

  m.rms_expected = rms(expected.data() + window_start, window_len);
  m.rms_dropped = rms(dropped.data() + window_start, window_len);

  double best_xcorr = -1e30;
  int best_lag = 0;
  for(int lag = -max_lag; lag <= max_lag; lag++)
  {
    int es = window_start;
    int ds = window_start + lag;
    if(ds < 0)
    {
      es += -ds;
      ds = 0;
    }
    const int len = std::min(
        window_len,
        std::min(int(expected.size()) - es, int(dropped.size()) - ds));
    if(len <= 0)
      continue;
    long double s = 0;
    long double e_sq = 0;
    long double d_sq = 0;
    for(int i = 0; i < len; i++)
    {
      const double ev = expected[es + i];
      const double dv = dropped[ds + i];
      s += (long double)ev * (long double)dv;
      e_sq += (long double)ev * (long double)ev;
      d_sq += (long double)dv * (long double)dv;
    }
    const double denom = std::sqrt(double(e_sq) * double(d_sq));
    if(denom <= 0)
      continue;
    const double xcorr = double(s) / denom;
    if(xcorr > best_xcorr)
    {
      best_xcorr = xcorr;
      best_lag = lag;
    }
  }
  m.best_lag_samples = best_lag;
  m.best_correlation = best_xcorr;

  // RMS of (expected[i] - dropped[i + best_lag]) over the window
  long double diff_sq = 0;
  int count = 0;
  for(int i = 0; i < window_len; i++)
  {
    const int ei = window_start + i;
    const int di = window_start + i + best_lag;
    if(di < 0 || di >= (int)dropped.size())
      continue;
    if(ei < 0 || ei >= (int)expected.size())
      continue;
    const double d = double(expected[ei]) - double(dropped[di]);
    diff_sq += (long double)d * (long double)d;
    count++;
  }
  m.rms_difference_at_best_lag = std::sqrt(double(diff_sq / std::max(1, count)));
  return m;
}

inline constexpr int64_t samples_to_flicks(int64_t samples, int sampleRate)
{
  return samples * (ossia::flicks_per_second<int64_t> / sampleRate);
}

inline std::vector<float>
run_stretcher_continuously(
    ossia::nodes::sound_ref& snd, ossia::execution_state& e,
    int64_t start_sample, int64_t end_sample, int bufferSize, int sampleRate,
    double tempo)
{
  std::vector<float> out;
  out.reserve(end_sample - start_sample);

  const int64_t buffer_flicks = samples_to_flicks(bufferSize, sampleRate);

  for(int64_t cursor = start_sample; cursor + bufferSize <= end_sample;
      cursor += bufferSize)
  {
    const int64_t cursor_flicks = samples_to_flicks(cursor, sampleRate);
    const int64_t end_flicks = samples_to_flicks(end_sample, sampleRate);
    ossia::token_request tk{
        ossia::time_value{cursor_flicks},
        ossia::time_value{cursor_flicks + buffer_flicks},
        ossia::time_value{end_flicks},
        ossia::time_value{0},
        1.0,
        {4, 4},
        tempo};

    snd.run(tk, ossia::exec_state_facade{&e});

    auto& ch = snd.audio_out.data.channel(0);
    for(int i = 0; i < bufferSize; i++)
      out.push_back(i < (int)ch.size() ? float(ch[i]) : 0.f);
  }
  return out;
}

inline std::unique_ptr<ossia::nodes::sound_ref>
make_sined_node(
    const ossia::audio_array& data, ossia::audio_stretch_mode mode,
    int sampleRate, int channels, double tempo)
{
  auto n = std::make_unique<ossia::nodes::sound_ref>();
  n->set_sound(data);
  // The 1-arg set_sound() forces stretch mode to None; override it here.
  n->m_resampler.reset(0, mode, channels, sampleRate);
  n->set_native_tempo(tempo);
  return n;
}
} // namespace

TEST_CASE("rubberband_drop_sync_unit_ratio", "[rubberband][sync]")
{
  using namespace ossia;

  // graph_node dtor asserts on thread kind.
  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);

  constexpr int sampleRate = 44100;
  constexpr int bufferSize = 256;
  constexpr int channels = 1;
  constexpr double tempo = 120.0;
  constexpr int64_t T_drop = 24064;
  constexpr int64_t T_measure = 16384;
  constexpr int64_t T_total = T_drop + T_measure + bufferSize;

  // 1 kHz sine modulated by a 5 Hz AM envelope so xcorr has a unique peak.
  const int64_t file_samples = T_total + 8192;
  audio_array data;
  data.resize(1);
  data[0].resize(file_samples);
  const double freq = 1000.0;
  const double env_freq = 5.0;
  for(int64_t i = 0; i < file_samples; i++)
  {
    const double env
        = 0.5 + 0.5 * std::sin(2.0 * M_PI * env_freq * double(i) / sampleRate);
    data[0][i] = 0.5f * float(std::sin(2.0 * M_PI * freq * double(i) / sampleRate) * env);
  }

  execution_state e;
  e.bufferSize = bufferSize;
  e.sampleRate = sampleRate;
  e.modelToSamplesRatio = double(sampleRate) / double(flicks_per_second<int64_t>);

  auto run_case = [&](const char* label, audio_stretch_mode mode) {
    auto A = make_sined_node(data, mode, sampleRate, channels, tempo);
    auto B = make_sined_node(data, mode, sampleRate, channels, tempo);

    const auto outA = run_stretcher_continuously(*A, e, 0, T_total, bufferSize, sampleRate, tempo);

    // Mimic add_time_process()'s transport(m_date) call.
    B->transport(ossia::time_value{samples_to_flicks(T_drop, sampleRate)});
    const auto outB = run_stretcher_continuously(
        *B, e, T_drop, T_total, bufferSize, sampleRate, tempo);

    REQUIRE(outA.size() >= (size_t)(T_drop + T_measure));
    REQUIRE(outB.size() >= (size_t)T_measure);

    auto transient = measure_sync(
        std::vector<float>(outA.begin() + T_drop, outA.end()),
        outB, 0, 1024, 1024);
    auto steady = measure_sync(
        std::vector<float>(outA.begin() + T_drop, outA.end()),
        outB, (int)T_measure - 4096, 4096, 1024);

    std::fprintf(
        stderr,
        "\n[%s] transient: lag=%d samples (%.2f ms)  xcorr=%.4f  "
        "rmsA=%.4f rmsB=%.4f diff=%.4f\n",
        label, transient.best_lag_samples,
        1000.0 * transient.best_lag_samples / sampleRate,
        transient.best_correlation, transient.rms_expected,
        transient.rms_dropped, transient.rms_difference_at_best_lag);
    std::fprintf(
        stderr,
        "[%s] steady:    lag=%d samples (%.2f ms)  xcorr=%.4f  "
        "rmsA=%.4f rmsB=%.4f diff=%.4f\n",
        label, steady.best_lag_samples,
        1000.0 * steady.best_lag_samples / sampleRate,
        steady.best_correlation, steady.rms_expected, steady.rms_dropped,
        steady.rms_difference_at_best_lag);

    return steady;
  };

  // None/Repitch reference points: no OLA reconstruction.
  run_case("none      ", audio_stretch_mode::None);
#if defined(OSSIA_ENABLE_LIBSAMPLERATE)
  run_case("repitch   ", audio_stretch_mode::Repitch);
#endif

  using ps = rubberband_stretcher::prime_strategy;
  struct
  {
    const char* name;
    ps strat;
  } variants[] = {
      {"rb_noPrime    ", ps::NoPrime},
      {"rb_zeroPadOnly", ps::ZeroPadOnly},
      {"rb_bareRecipe ", ps::BareRecipe},
      {"rb_extendDrain", ps::ExtendedDrain},
      {"rb_preRoll    ", ps::PreRollRealAudio},
  };

  sync_measurement best{};
  best.best_correlation = -1.0;
  const char* best_name = nullptr;
  for(auto& v : variants)
  {
    rubberband_stretcher::s_prime_strategy = v.strat;
    auto s = run_case(v.name, audio_stretch_mode::RubberBandStandard);
    if(s.best_correlation > best.best_correlation)
    {
      best = s;
      best_name = v.name;
    }
  }
  std::fprintf(
      stderr,
      "\n>>> best @ ratio=1.0: %s with xcorr=%.4f, lag=%d samples (%.2f ms)\n",
      best_name ? best_name : "?", best.best_correlation,
      best.best_lag_samples,
      1000.0 * best.best_lag_samples / sampleRate);

  rubberband_stretcher::s_prime_strategy = ps::ZeroPadOnly;

  // CHECK not REQUIRE so failure still prints measurements.
  CHECK(best.best_correlation > 0.9);
}

TEST_CASE("rubberband_drop_sync_tempo_shift", "[rubberband][sync]")
{
  // file=100 BPM, timeline=120 BPM (stretch ratio 100/120 ≈ 0.833).
  using namespace ossia;

  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);

  constexpr int sampleRate = 44100;
  constexpr int bufferSize = 256;
  constexpr int channels = 1;
  constexpr double file_tempo = 100.0;
  constexpr double timeline_tempo = 120.0;
  constexpr int64_t T_drop = 24064;
  constexpr int64_t T_measure = 16384;
  constexpr int64_t T_total = T_drop + T_measure + bufferSize;

  const int64_t file_samples = T_total + 32768;
  audio_array data;
  data.resize(1);
  data[0].resize(file_samples);
  const double freq = 1000.0;
  const double env_freq = 5.0;
  for(int64_t i = 0; i < file_samples; i++)
  {
    const double env
        = 0.5 + 0.5 * std::sin(2.0 * M_PI * env_freq * double(i) / sampleRate);
    data[0][i] = 0.5f * float(std::sin(2.0 * M_PI * freq * double(i) / sampleRate) * env);
  }

  execution_state e;
  e.bufferSize = bufferSize;
  e.sampleRate = sampleRate;
  e.modelToSamplesRatio = double(sampleRate) / double(flicks_per_second<int64_t>);

  // via_sampler = legacy bug path; stretch_aware = the new overload.
  enum class seek_mode
  {
    via_sampler,
    stretch_aware,
  };

  auto run_case = [&](const char* label, audio_stretch_mode mode, seek_mode sm) {
    auto A = make_sined_node(data, mode, sampleRate, channels, file_tempo);
    auto B = make_sined_node(data, mode, sampleRate, channels, file_tempo);

    const auto outA = run_stretcher_continuously(
        *A, e, 0, T_total, bufferSize, sampleRate, timeline_tempo);

    if(sm == seek_mode::via_sampler)
    {
      B->transport(ossia::time_value{samples_to_flicks(T_drop, sampleRate)});
    }
    else
    {
      ossia::tick_transport_info tinfo{};
      tinfo.current_tempo = timeline_tempo;
      B->transport(ossia::time_value{samples_to_flicks(T_drop, sampleRate)}, tinfo);
    }
    const auto outB = run_stretcher_continuously(
        *B, e, T_drop, T_total, bufferSize, sampleRate, timeline_tempo);

    REQUIRE(outA.size() >= (size_t)(T_drop + T_measure));
    REQUIRE(outB.size() >= (size_t)T_measure);

    auto steady = measure_sync(
        std::vector<float>(outA.begin() + T_drop, outA.end()),
        outB, (int)T_measure - 4096, 4096, 2048);

    std::fprintf(
        stderr,
        "\n[tempo %.0f→%.0f, %s] steady: lag=%d samples (%.2f ms)  "
        "xcorr=%.4f  rmsA=%.4f rmsB=%.4f diff=%.4f\n",
        file_tempo, timeline_tempo, label, steady.best_lag_samples,
        1000.0 * steady.best_lag_samples / sampleRate,
        steady.best_correlation, steady.rms_expected, steady.rms_dropped,
        steady.rms_difference_at_best_lag);

    return steady;
  };

  std::fprintf(stderr, "\n--- tempo-shift, via_sampler seek (production path) ---\n");
  run_case("none      ", audio_stretch_mode::None, seek_mode::via_sampler);
#if defined(OSSIA_ENABLE_LIBSAMPLERATE)
  run_case("repitch   ", audio_stretch_mode::Repitch, seek_mode::via_sampler);
#endif

  using ps = rubberband_stretcher::prime_strategy;
  struct
  {
    const char* name;
    ps strat;
  } variants[] = {
      {"rb_noPrime    ", ps::NoPrime},
      {"rb_zeroPadOnly", ps::ZeroPadOnly},
      {"rb_bareRecipe ", ps::BareRecipe},
      {"rb_extendDrain", ps::ExtendedDrain},
      {"rb_preRoll    ", ps::PreRollRealAudio},
  };

  auto run_strategies = [&](seek_mode sm) {
    sync_measurement best{};
    best.best_correlation = -1.0;
    const char* best_name = nullptr;
    for(auto& v : variants)
    {
      rubberband_stretcher::s_prime_strategy = v.strat;
      auto s = run_case(v.name, audio_stretch_mode::RubberBandStandard, sm);
      if(s.best_correlation > best.best_correlation)
      {
        best = s;
        best_name = v.name;
      }
    }
    std::fprintf(
        stderr,
        "\n>>> best %s: %s with xcorr=%.4f, lag=%d samples (%.2f ms)\n",
        sm == seek_mode::via_sampler ? "via_sampler" : "stretch_aware",
        best_name ? best_name : "?", best.best_correlation,
        best.best_lag_samples,
        1000.0 * best.best_lag_samples / sampleRate);
  };

  run_strategies(seek_mode::via_sampler);

  std::fprintf(stderr, "\n--- tempo-shift, stretch_aware seek (proposed fix) ---\n");
  run_case("none      ", audio_stretch_mode::None, seek_mode::stretch_aware);
#if defined(OSSIA_ENABLE_LIBSAMPLERATE)
  run_case("repitch   ", audio_stretch_mode::Repitch, seek_mode::stretch_aware);
#endif
  run_strategies(seek_mode::stretch_aware);

  rubberband_stretcher::s_prime_strategy = ps::ZeroPadOnly;
}

TEST_CASE("rubberband_drop_then_paste_sync", "[rubberband][sync]")
{
  // Scenario A: both nodes are added during playback (drop + paste).
  using namespace ossia;

  ossia::set_thread_pinned(ossia::thread_type::Ui, 0);

  constexpr int sampleRate = 44100;
  constexpr int bufferSize = 256;
  constexpr int channels = 1;
  constexpr double file_tempo = 100.0;
  constexpr double timeline_tempo = 120.0;
  constexpr int64_t T_drop = 24064;
  constexpr int64_t T_paste = 48128;
  constexpr int64_t T_measure = 16384;
  constexpr int64_t T_total = T_paste + T_measure + bufferSize;

  const int64_t file_samples = T_total + 32768;
  audio_array data;
  data.resize(1);
  data[0].resize(file_samples);
  const double freq = 1000.0;
  const double env_freq = 5.0;
  for(int64_t i = 0; i < file_samples; i++)
  {
    const double env
        = 0.5 + 0.5 * std::sin(2.0 * M_PI * env_freq * double(i) / sampleRate);
    data[0][i]
        = 0.5f * float(std::sin(2.0 * M_PI * freq * double(i) / sampleRate) * env);
  }

  execution_state e;
  e.bufferSize = bufferSize;
  e.sampleRate = sampleRate;
  e.modelToSamplesRatio = double(sampleRate) / double(flicks_per_second<int64_t>);

  auto run_case = [&](const char* label, audio_stretch_mode mode) {
    auto orig = make_sined_node(data, mode, sampleRate, channels, file_tempo);
    auto paste = make_sined_node(data, mode, sampleRate, channels, file_tempo);

    ossia::tick_transport_info tinfo{};
    tinfo.current_tempo = timeline_tempo;

    orig->transport(ossia::time_value{samples_to_flicks(T_drop, sampleRate)}, tinfo);
    const int64_t orig_after_seek = orig->m_resampler.next_sample_to_read();

    auto outOrig_pre = run_stretcher_continuously(
        *orig, e, T_drop, T_paste, bufferSize, sampleRate, timeline_tempo);
    const int64_t orig_at_T_paste = orig->m_resampler.next_sample_to_read();

    auto outOrig_post = run_stretcher_continuously(
        *orig, e, T_paste, T_total, bufferSize, sampleRate, timeline_tempo);
    std::vector<float> outOrig;
    outOrig.reserve(outOrig_pre.size() + outOrig_post.size());
    outOrig.insert(outOrig.end(), outOrig_pre.begin(), outOrig_pre.end());
    outOrig.insert(outOrig.end(), outOrig_post.begin(), outOrig_post.end());

    paste->transport(ossia::time_value{samples_to_flicks(T_paste, sampleRate)}, tinfo);
    const int64_t paste_after_seek = paste->m_resampler.next_sample_to_read();
    const auto outPaste = run_stretcher_continuously(
        *paste, e, T_paste, T_total, bufferSize, sampleRate, timeline_tempo);

    std::fprintf(
        stderr,
        "  [%s] file_pos: orig_seek=%lld  orig@T_paste=%lld  paste_seek=%lld  diff=%lld\n",
        label, (long long)orig_after_seek, (long long)orig_at_T_paste,
        (long long)paste_after_seek,
        (long long)(orig_at_T_paste - paste_after_seek));

    REQUIRE(outOrig.size() >= (size_t)(T_paste - T_drop + T_measure));
    REQUIRE(outPaste.size() >= (size_t)T_measure);

    const std::vector<float> expected(
        outOrig.begin() + (T_paste - T_drop), outOrig.end());
    auto steady = measure_sync(
        expected, outPaste, (int)T_measure - 4096, 4096, 2048);

    std::fprintf(
        stderr,
        "[drop+paste, %s] steady: lag=%d samples (%.2f ms)  xcorr=%.4f  "
        "rmsA=%.4f rmsB=%.4f diff=%.4f\n",
        label, steady.best_lag_samples,
        1000.0 * steady.best_lag_samples / sampleRate,
        steady.best_correlation, steady.rms_expected, steady.rms_dropped,
        steady.rms_difference_at_best_lag);
    return steady;
  };

  std::fprintf(stderr,
      "\n--- drop-during-playback + paste-during-playback (scenario A) ---\n");
  run_case("none      ", audio_stretch_mode::None);
#if defined(OSSIA_ENABLE_LIBSAMPLERATE)
  run_case("repitch   ", audio_stretch_mode::Repitch);
#endif

  using ps = rubberband_stretcher::prime_strategy;
  struct
  {
    const char* name;
    ps strat;
  } variants[] = {
      {"rb_zeroPadOnly", ps::ZeroPadOnly},
      {"rb_bareRecipe ", ps::BareRecipe},
      {"rb_extendDrain", ps::ExtendedDrain},
      {"rb_preRoll    ", ps::PreRollRealAudio},
  };

  for(auto& v : variants)
  {
    rubberband_stretcher::s_prime_strategy = v.strat;
    run_case(v.name, audio_stretch_mode::RubberBandStandard);
  }

  rubberband_stretcher::s_prime_strategy = ps::ZeroPadOnly;
}
#endif
