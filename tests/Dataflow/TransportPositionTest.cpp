// The transport position a node reads while it runs: that of the first sample
// of the tick being run, whichever graph runs it, through a play from 0, a
// tick that starts inside its buffer, a loop, a seek, other speeds and
// backwards play; and at any speed, the one the musical position and the
// tempo of the tick say.
#include <ossia/detail/config.hpp>

#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

#include "include_catch.hpp"

#include "../Editor/TestUtils.hpp"

#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include <vector>

namespace
{
constexpr double flicks_ratio_48k = 48000. / ossia::flicks_per_second<double>;

//! What a plug-in host reads from the node while it runs
struct position_probe final : public ossia::graph_node
{
  std::vector<int64_t> positions;
  std::vector<ossia::token_request> requests;
  position_probe()
  {
    m_inlets.push_back(new ossia::audio_inlet);
    m_outlets.push_back(new ossia::audio_outlet);
  }
  std::string label() const noexcept override { return "position probe"; }
  void run(const ossia::token_request& tk, ossia::exec_state_facade) noexcept override
  {
    positions.push_back(transport_frames());
    requests.push_back(tk);
  }
};

struct tick_t
{
  int64_t prev, date; // in samples
  int64_t start_sample, length;
  double speed;
};

// A play from 0, a tick starting inside its buffer, a loop back to 0, a seek,
// twice and half the speed, then backwards
const tick_t ticks[]{
    {0, 512, 0, 512, 1.},          {512, 1024, 0, 512, 1.},
    {1024, 1388, 148, 364, 1.},    {1388, 1900, 0, 512, 1.},
    {0, 512, 0, 512, 1.},          {48000, 48512, 0, 512, 1.},
    {48512, 49536, 0, 512, 2.},    {49536, 49792, 0, 512, 0.5},
    {49792, 49280, 0, 512, -1.},   {49280, 48768, 0, 512, -1.}};

ossia::token_request request(const tick_t& t)
{
  ossia::token_request tk;
  tk.prev_date = ossia::time_value{int64_t(t.prev / flicks_ratio_48k)};
  tk.date = ossia::time_value{int64_t(t.date / flicks_ratio_48k)};
  tk.start_sample = t.start_sample;
  tk.length_sample = t.length;
  tk.speed = t.speed;
  tk.tempo = 120.;
  return tk;
}

//! The frames the timeline would have played up to the tick's first sample at
//! the tick's speed: the frames the tick's tempo, which carries the speed,
//! turns into its musical position
std::vector<int64_t> expected_positions()
{
  std::vector<int64_t> res;
  for(const auto& t : ticks)
    res.push_back(int64_t(t.prev / (t.speed < 0 ? -t.speed : t.speed)));
  return res;
}

void setup_state(ossia::execution_state& e)
{
  e.bufferSize = 512;
  e.sampleRate = 48000;
  e.modelToSamplesRatio = flicks_ratio_48k;
  e.samplesToModelRatio = 1. / flicks_ratio_48k;
}
}


TEST_CASE("a_node_reads_the_position_of_the_tick_it_runs", "[transport]")
{
  using opt = ossia::graph_setup_options;
  for(auto scheduling : {opt::StaticFixed, opt::StaticBFS, opt::StaticTC, opt::Dynamic})
    for(bool parallel : {false, true})
      for(bool logged : {false, true})
      {
        CAPTURE(int(scheduling), parallel, logged);
        opt options;
        options.scheduling = scheduling;
        options.parallel = parallel;
        options.parallel_threads = 2;
        if(logged)
          options.log = std::make_shared<spdlog::logger>(
              "transport", std::make_shared<spdlog::sinks::null_sink_mt>());
        auto g = ossia::make_graph(options);

        auto probe = std::make_shared<position_probe>();
        g->add_node(probe);

        ossia::execution_state e;
        setup_state(e);
        for(const auto& t : ticks)
        {
          probe->request(request(t));
          e.begin_tick();
          g->state(e);
          e.commit();
        }

        // Within a sample of the exact one: the dates are whole flicks
        REQUIRE(probe->positions.size() == std::size(ticks));
        const auto expected = expected_positions();
        for(std::size_t i = 0; i < expected.size(); i++)
        {
          CAPTURE(i, probe->positions[i], expected[i]);
          CHECK(std::abs(probe->positions[i] - expected[i]) <= 1);
        }
        g->clear();
      }
}

TEST_CASE("the_position_agrees_with_the_musical_position_at_any_speed", "[transport]")
{
  // An interval with a meter, played at a speed: the tick's tempo is the
  // interval's times the speed, and the position, the musical position and
  // that tempo must tell the same story, tick after tick, as a plug-in
  // turning one into the other expects
  for(double speed : {1., 2., 0.5, 1.5, -1.})
  {
    CAPTURE(speed);
    ossia::execution_state e;
    setup_state(e);
    root_scenario s;
    auto se = *s.scenario->get_start_time_sync()->get_time_events().begin();
    auto en = std::make_shared<ossia::time_sync>();
    en->set_expression(ossia::expressions::make_expression_true());
    auto ee = std::make_shared<ossia::time_event>(
        ossia::time_event::exec_callback{}, *en,
        ossia::expressions::make_expression_true());
    en->insert(en->get_time_events().end(), ee);
    s.scenario->add_time_sync(en);
    auto itv = ossia::time_interval::create(
        {}, *se, *ee, ossia::Infinite, ossia::Infinite, ossia::Infinite);
    ossia::time_signature_map m;
    m[ossia::time_value{0}] = {4, 4};
    itv->set_time_signature_map(m);
    // A quarter is half a second: 120 BPM at speed 1
    itv->set_quarter_duration(ossia::flicks_per_second<double> / 2.);
    s.scenario->add_time_interval(itv);
    auto probe = std::make_shared<position_probe>();
    itv->add_time_process(std::make_shared<ossia::node_process>(probe));
    itv->set_speed(speed);
    s.interval->start();
    s.interval->tick_current(ossia::time_value{}, {});

    ossia::token_request root;
    root.tempo = 120;
    root.speed = 1.;
    root.signature = {4, 4};
    root.start_sample = 0;
    root.length_sample = 512;
    const int64_t buffer_flicks = int64_t(512 / flicks_ratio_48k);

    // Backwards from 10 s in
    if(speed < 0)
    {
      itv->set_speed(1.);
      for(int i = 0; i < 1000; i++)
        s.interval->tick(ossia::time_value{buffer_flicks}, root);
      probe->requested_tokens.clear();
      itv->set_speed(speed);
    }

    auto g = ossia::make_graph({});
    g->add_node(probe);
    for(int i = 0; i < 16; i++)
    {
      probe->requested_tokens.clear();
      s.interval->tick(ossia::time_value{buffer_flicks}, root);
      e.begin_tick();
      g->state(e);
      e.commit();
    }

    REQUIRE(probe->requests.size() >= 16);
    for(std::size_t i = 0; i < probe->requests.size(); i++)
    {
      const auto& tk = probe->requests[i];
      // Played backwards, the tempo is negative: the beats still count from 0
      const double tempo = tk.tempo < 0 ? -tk.tempo : tk.tempo;
      const double beats_from_position
          = double(probe->positions[i]) / e.sampleRate * (tempo / 60.);
      CAPTURE(i, probe->positions[i], tk.tempo, tk.musical_start_position);
      // Within a sample's worth of beats
      CHECK(
          std::abs(beats_from_position - tk.musical_start_position)
          <= tempo / 60. / e.sampleRate + 1e-9);
    }
    g->clear();
  }
}
