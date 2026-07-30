// Scenario-level tests for backwards playback (speed < 0).
#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/nodes/dummy.hpp>
#include <ossia/dataflow/nodes/sound_ref.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include "include_catch.hpp"

#include "../Editor/TestUtils.hpp"

#include <algorithm>
#include <vector>

namespace
{
auto create_event(ossia::scenario& s)
{
  auto en = std::make_shared<ossia::time_sync>();
  en->set_expression(ossia::expressions::make_expression_true());
  auto ee = std::make_shared<ossia::time_event>(
      ossia::time_event::exec_callback{}, *en,
      ossia::expressions::make_expression_true());
  en->insert(en->get_time_events().end(), ee);
  s.add_time_sync(std::move(en));
  return ee;
}

std::shared_ptr<ossia::time_interval> create_interval(
    ossia::time_event& startEvent, ossia::time_event& endEvent, ossia::time_value d)
{
  auto ptr = ossia::time_interval::create({}, startEvent, endEvent, d, d, d);
  ptr->add_time_process(std::make_shared<ossia::node_process>(
      std::make_shared<ossia::nodes::dummy_node>()));
  return ptr;
}

auto start_event(ossia::scenario& s)
{
  auto sn = s.get_start_time_sync();
  return *sn->get_time_events().begin();
}

ossia::token_request default_request()
{
  ossia::token_request req;
  req.tempo = 120;
  req.speed = 1.;
  req.signature = {4, 4};
  return req;
}

void setup_state(ossia::execution_state& e, int bufferSize)
{
  e.bufferSize = bufferSize;
  e.sampleRate = 48000;
  e.modelToSamplesRatio = 48000. / ossia::flicks_per_second<double>;
  e.samplesToModelRatio = ossia::flicks_per_second<double> / 48000.;
}

//! Records the buffer span every tick hands it, the way every audio plug-in
//! node does.
struct probe_node final : public ossia::graph_node
{
  struct span
  {
    int64_t start{}, frames{};
    int64_t prev_date{}, date{};
  };
  std::vector<span> spans;

  probe_node()
  {
    m_inlets.push_back(new ossia::audio_inlet);
    m_outlets.push_back(new ossia::audio_outlet);
  }
  std::string label() const noexcept override { return "probe"; }

  void run(const ossia::token_request& tk, ossia::exec_state_facade st) noexcept override
  {
    auto [start, frames] = st.timings(tk);
    spans.push_back({start, frames, tk.prev_date.impl, tk.date.impl});
  }
};

//! An audio effect the way CLAP / ysfx / avnd / (now) VST / VST3 / LV2 / faust
//! do it: honour timings(), copy the span through.
struct plugin_fx final : public ossia::nonowning_graph_node
{
  ossia::audio_inlet in;
  ossia::audio_outlet out;
  plugin_fx()
  {
    m_inlets.push_back(&in);
    m_outlets.push_back(&out);
  }
  std::string label() const noexcept override { return "plugin_fx"; }

  void run(const ossia::token_request& tk, ossia::exec_state_facade st) noexcept override
  {
    if(tk.paused())
      return;
    auto [start, frames] = st.timings(tk);
    if(frames <= 0)
      return;

    auto& ip = *in;
    auto& op = *out;
    op.set_channels(std::max<std::size_t>(1, ip.channels()));
    for(std::size_t c = 0; c < op.channels(); c++)
    {
      op.channel(c).resize(st.bufferSize());
      if(c >= ip.channels())
        continue;
      ip.channel(c).resize(st.bufferSize());
      for(int64_t i = start; i < start + frames; i++)
        op.channel(c)[i] = ip.channel(c)[i];
    }
  }
};

//! Stands in for the interval / root nodes that keep ticking whatever the inner
//! intervals do.
struct sink_node final : public ossia::nonowning_graph_node
{
  ossia::audio_inlet in;
  ossia::audio_outlet out;
  std::vector<double> last;
  sink_node()
  {
    m_inlets.push_back(&in);
    m_outlets.push_back(&out);
  }
  std::string label() const noexcept override { return "sink"; }
  void run(const ossia::token_request&, ossia::exec_state_facade) noexcept override
  {
    last.clear();
    auto& ip = *in;
    if(ip.channels() > 0)
      for(auto v : ip.channel(0))
        last.push_back(v);
  }
};

//! The spans claimed on one tick must tile [0 ; bufferSize[ exactly.
void require_exact_coverage(std::vector<probe_node::span> spans, int64_t bufferSize)
{
  std::sort(spans.begin(), spans.end(), [](const auto& a, const auto& b) {
    return a.start < b.start;
  });

  int64_t expected = 0;
  for(const auto& s : spans)
  {
    CAPTURE(s.start, s.frames, s.prev_date, s.date);
    REQUIRE(s.frames > 0);
    REQUIRE(s.start == expected);
    expected = s.start + s.frames;
  }
  REQUIRE(expected == bufferSize);
}
}

TEST_CASE("test_backward_steady_state_timings", "test_backward_steady_state_timings")
{
  using namespace ossia;

  constexpr int bs = 256;
  execution_state e;
  setup_state(e, bs);
  const int64_t buffer_flicks = int64_t(bs * e.samplesToModelRatio);

  root_scenario s;
  auto se = start_event(*s.scenario);
  auto ev1 = create_event(*s.scenario);
  auto c0 = create_interval(*se, *ev1, ossia::time_value{200 * buffer_flicks});
  s.scenario->add_time_interval(c0);

  auto probe = std::make_shared<probe_node>();
  c0->add_time_process(std::make_shared<ossia::node_process>(probe));

  s.interval->start();
  s.interval->tick_current(ossia::time_value{}, {});

  auto do_tick = [&] {
    probe->requested_tokens.clear();
    s.interval->tick(ossia::time_value{buffer_flicks}, default_request());
    for(auto& tk : probe->requested_tokens)
      probe->run(tk, {&e});
  };

  for(int i = 0; i < 4; i++)
    do_tick();
  s.interval->set_speed(-1.);
  for(int i = 0; i < 4; i++)
    do_tick();

  REQUIRE(probe->spans.size() == 8);

  // Every tick, forward or backward, covers the whole buffer from its start.
  for(std::size_t i = 0; i < probe->spans.size(); i++)
  {
    CAPTURE(i);
    REQUIRE(probe->spans[i].start == 0);
    REQUIRE(probe->spans[i].frames == bs);
  }

  // Forward the dates go up by one buffer, backward they come back down the
  // same way, and the rewind lands exactly where the forward pass started.
  for(int i = 0; i < 4; i++)
  {
    CAPTURE(i);
    REQUIRE(probe->spans[i].date - probe->spans[i].prev_date == buffer_flicks);
  }
  for(int i = 4; i < 8; i++)
  {
    CAPTURE(i);
    REQUIRE(probe->spans[i].prev_date - probe->spans[i].date == buffer_flicks);
  }
  REQUIRE(probe->spans[3].date == probe->spans[4].prev_date);
  REQUIRE(probe->spans[7].date == 0);
}

TEST_CASE("test_backward_boundary_buffer_coverage", "test_backward_boundary_buffer_coverage")
{
  using namespace ossia;

  constexpr int bs = 256;
  execution_state e;
  setup_state(e, bs);
  const int64_t buffer_flicks = int64_t(bs * e.samplesToModelRatio);

  // 2.5 buffers each, so both boundaries fall in the middle of a buffer.
  const auto dur = ossia::time_value{5 * buffer_flicks / 2};

  root_scenario s;
  auto se = start_event(*s.scenario);
  auto ev1 = create_event(*s.scenario);
  auto ev2 = create_event(*s.scenario);
  auto c0 = create_interval(*se, *ev1, dur);
  s.scenario->add_time_interval(c0);
  auto c1 = create_interval(*ev1, *ev2, dur);
  s.scenario->add_time_interval(c1);

  auto p0 = std::make_shared<probe_node>();
  auto p1 = std::make_shared<probe_node>();
  c0->add_time_process(std::make_shared<ossia::node_process>(p0));
  c1->add_time_process(std::make_shared<ossia::node_process>(p1));

  s.interval->start();
  s.interval->tick_current(ossia::time_value{}, {});

  // Returns the spans claimed on this tick, per interval.
  auto do_tick = [&] {
    p0->requested_tokens.clear();
    p1->requested_tokens.clear();
    p0->spans.clear();
    p1->spans.clear();
    s.interval->tick(ossia::time_value{buffer_flicks}, default_request());
    for(auto& tk : p0->requested_tokens)
      p0->run(tk, {&e});
    for(auto& tk : p1->requested_tokens)
      p1->run(tk, {&e});

    std::vector<probe_node::span> all;
    all.insert(all.end(), p0->spans.begin(), p0->spans.end());
    all.insert(all.end(), p1->spans.begin(), p1->spans.end());
    return all;
  };

  // ---- Forward: 4 ticks, the boundary lands inside tick 3.
  int forward_boundary_ticks = 0;
  for(int i = 0; i < 4; i++)
  {
    CAPTURE("forward", i);
    const auto all = do_tick();
    REQUIRE(!all.empty());
    require_exact_coverage(all, bs);
    if(all.size() == 2)
    {
      forward_boundary_ticks++;
      // The interval that is earlier in the timeline gets the earlier part of
      // the buffer.
      REQUIRE(p0->spans.size() == 1);
      REQUIRE(p1->spans.size() == 1);
      REQUIRE(p0->spans[0].start == 0);
      REQUIRE(p1->spans[0].start == p0->spans[0].frames);
    }
  }
  REQUIRE(forward_boundary_ticks == 1);

  // ---- Backward over the same ground.
  s.interval->set_speed(-1.);
  int backward_boundary_ticks = 0;
  for(int i = 0; i < 4; i++)
  {
    CAPTURE("backward", i);
    const auto all = do_tick();
    REQUIRE(!all.empty());
    require_exact_coverage(all, bs);
    if(all.size() == 2)
    {
      backward_boundary_ticks++;
      // Rewinding, the interval that is *later* in the timeline is the one
      // heard first, so it takes the earlier part of the buffer.
      REQUIRE(p0->spans.size() == 1);
      REQUIRE(p1->spans.size() == 1);
      REQUIRE(p1->spans[0].start == 0);
      REQUIRE(p0->spans[0].start == p1->spans[0].frames);
    }
  }
  REQUIRE(backward_boundary_ticks == 1);
}

TEST_CASE("test_backward_multiple_boundaries_in_one_buffer",
          "test_backward_multiple_boundaries_in_one_buffer")
{
  using namespace ossia;

  // Intervals shorter than a buffer, so a single tick crosses several of them
  // and the overtick cascades more than once. Each cascade narrows the tick it
  // hands down, so the buffer offset has to be derived from the whole tick and
  // not from the already-narrowed one.
  constexpr int bs = 256;
  constexpr int n_itv = 6;
  execution_state e;
  setup_state(e, bs);
  const int64_t buffer_flicks = int64_t(bs * e.samplesToModelRatio);
  // 3/8th of a buffer each: one tick crosses two boundaries and stops inside
  // the third interval, in both directions.
  const auto dur = ossia::time_value{3 * buffer_flicks / 8};

  root_scenario s;
  std::vector<std::shared_ptr<time_event>> evs;
  evs.push_back(start_event(*s.scenario));
  for(int i = 0; i < n_itv; i++)
    evs.push_back(create_event(*s.scenario));

  std::vector<std::shared_ptr<probe_node>> probes;
  for(int i = 0; i < n_itv; i++)
  {
    auto itv = create_interval(*evs[i], *evs[i + 1], dur);
    s.scenario->add_time_interval(itv);
    auto p = std::make_shared<probe_node>();
    probes.push_back(p);
    itv->add_time_process(std::make_shared<ossia::node_process>(p));
  }

  s.interval->start();
  s.interval->tick_current(ossia::time_value{}, {});

  auto do_tick = [&] {
    std::vector<probe_node::span> all;
    for(auto& p : probes)
    {
      p->requested_tokens.clear();
      p->spans.clear();
    }
    s.interval->tick(ossia::time_value{buffer_flicks}, default_request());
    for(auto& p : probes)
    {
      for(auto& tk : p->requested_tokens)
        p->run(tk, {&e});
      all.insert(all.end(), p->spans.begin(), p->spans.end());
    }
    return all;
  };

  // One forward buffer covers four of the six intervals.
  {
    const auto all = do_tick();
    CAPTURE(all.size());
    REQUIRE(all.size() >= 3);
    require_exact_coverage(all, bs);
  }

  // Rewind over the same ground: the boundaries must tile the buffer just the
  // same. This is where using the narrowed tick as the base put every interval
  // after the first cascade at the wrong buffer position.
  s.interval->set_speed(-1.);
  {
    const auto all = do_tick();
    CAPTURE(all.size());
    REQUIRE(all.size() >= 3);
    require_exact_coverage(all, bs);
  }
}

TEST_CASE("test_backward_audio_is_reversed_stream", "test_backward_audio_is_reversed_stream")
{
  using namespace ossia;

  constexpr int bs = 8;
  execution_state e;
  setup_state(e, bs);
  const int64_t buffer_flicks = int64_t(bs * e.samplesToModelRatio);

  // A 64-sample ramp holding 1..64, so an output sample tells us which file
  // sample it came from.
  ossia::audio_array data;
  data.resize(1);
  data[0].resize(64);
  for(int i = 0; i < 64; i++)
    data[0][i] = float(i + 1);

  auto snd = std::make_shared<ossia::nodes::sound_ref>();
  snd->set_sound(data);
  auto fx = std::make_shared<plugin_fx>();
  auto sink = std::make_shared<sink_node>();

  ossia::tc_graph g;
  g.add_node(snd);
  g.add_node(fx);
  g.add_node(sink);
  g.connect(g.allocate_edge(
      ossia::immediate_glutton_connection{}, &snd->audio_out, &fx->in, snd, fx));
  g.connect(g.allocate_edge(
      ossia::immediate_glutton_connection{}, &fx->out, &sink->in, fx, sink));

  root_scenario s;
  auto se = start_event(*s.scenario);
  auto ev1 = create_event(*s.scenario);
  auto c0 = create_interval(*se, *ev1, ossia::time_value{8 * buffer_flicks});
  s.scenario->add_time_interval(c0);
  c0->add_time_process(std::make_shared<ossia::node_process>(snd));
  c0->add_time_process(std::make_shared<ossia::node_process>(fx));
  s.interval->add_time_process(std::make_shared<ossia::node_process>(sink));

  s.interval->start();
  s.interval->tick_current(ossia::time_value{}, {});

  std::vector<std::vector<double>> buffers;
  auto do_tick = [&] {
    e.begin_tick();
    s.interval->tick(ossia::time_value{buffer_flicks}, default_request());
    g.state(e);
    e.commit();
    buffers.push_back(sink->last);
  };

  for(int i = 0; i < 5; i++)
    do_tick();
  s.interval->set_speed(-1.);
  for(int i = 0; i < 5; i++)
    do_tick();

  REQUIRE(buffers.size() == 10);
  for(std::size_t i = 0; i < buffers.size(); i++)
  {
    CAPTURE(i);
    REQUIRE(buffers[i].size() == bs);
  }

  // Forward, steady state: buffer n holds file samples 8n+1 .. 8n+8.
  // (buffer 0 is faded in - the interval just started - so it is checked
  // separately below.)
  for(int b = 1; b < 5; b++)
  {
    for(int i = 0; i < bs; i++)
    {
      CAPTURE(b, i);
      REQUIRE(buffers[b][i] == Catch::Approx(double(b * bs + i + 1)));
    }
  }

  // Backward, steady state: the exact same samples, in reverse. Buffer 5 is the
  // direction change itself, buffer 9 hits the start of the interval and is
  // faded out, so the unambiguous ones are 6, 7, 8.
  const double expected_first[3] = {33., 25., 17.};
  for(int b = 6; b <= 8; b++)
  {
    for(int i = 0; i < bs; i++)
    {
      CAPTURE(b, i);
      REQUIRE(buffers[b][i] == Catch::Approx(expected_first[b - 6] - i));
    }
  }

  // The stream is continuous across the buffer boundaries: the last sample of
  // one buffer is followed by its predecessor.
  for(int b = 6; b <= 8; b++)
  {
    CAPTURE(b);
    REQUIRE(buffers[b][0] == Catch::Approx(buffers[b - 1][bs - 1] - 1.));
  }

  // Direction change: buffer 5 is still strictly descending by one sample.
  for(int i = 1; i < bs; i++)
  {
    CAPTURE(i);
    REQUIRE(buffers[5][i] == Catch::Approx(buffers[5][i - 1] - 1.));
  }

  // Fades: the first and last buffers are attenuated versions of the ramp, so
  // they must stay bounded by it and never be silent all through.
  for(int b : {0, 9})
  {
    bool any = false;
    for(int i = 0; i < bs; i++)
    {
      CAPTURE(b, i);
      REQUIRE(std::abs(buffers[b][i]) <= 64.);
      any |= buffers[b][i] != 0.;
    }
    REQUIRE(any);
  }
}

TEST_CASE("test_backward_audio_plugin_is_not_silent", "test_backward_audio_plugin_is_not_silent")
{
  using namespace ossia;

  // The shared execution path must keep handing a node real work to do while
  // the timeline runs backwards.
  constexpr int bs = 8;
  execution_state e;
  setup_state(e, bs);
  const int64_t buffer_flicks = int64_t(bs * e.samplesToModelRatio);

  ossia::audio_array data;
  data.resize(1);
  data[0].resize(64);
  for(int i = 0; i < 64; i++)
    data[0][i] = 1.f;

  auto snd = std::make_shared<ossia::nodes::sound_ref>();
  snd->set_sound(data);
  auto fx = std::make_shared<plugin_fx>();
  auto sink = std::make_shared<sink_node>();

  ossia::tc_graph g;
  g.add_node(snd);
  g.add_node(fx);
  g.add_node(sink);
  g.connect(g.allocate_edge(
      ossia::immediate_glutton_connection{}, &snd->audio_out, &fx->in, snd, fx));
  g.connect(g.allocate_edge(
      ossia::immediate_glutton_connection{}, &fx->out, &sink->in, fx, sink));

  root_scenario s;
  auto se = start_event(*s.scenario);
  auto ev1 = create_event(*s.scenario);
  auto c0 = create_interval(*se, *ev1, ossia::time_value{8 * buffer_flicks});
  s.scenario->add_time_interval(c0);
  c0->add_time_process(std::make_shared<ossia::node_process>(snd));
  c0->add_time_process(std::make_shared<ossia::node_process>(fx));
  s.interval->add_time_process(std::make_shared<ossia::node_process>(sink));

  s.interval->start();
  s.interval->tick_current(ossia::time_value{}, {});

  auto do_tick = [&] {
    e.begin_tick();
    s.interval->tick(ossia::time_value{buffer_flicks}, default_request());
    g.state(e);
    e.commit();
  };

  for(int i = 0; i < 4; i++)
    do_tick();

  s.interval->set_speed(-1.);
  for(int i = 0; i < 3; i++)
  {
    CAPTURE(i);
    do_tick();
    REQUIRE(sink->last.size() == bs);
    double sum = 0.;
    for(double v : sink->last)
      sum += std::abs(v);
    REQUIRE(sum > 0.);
  }
}
