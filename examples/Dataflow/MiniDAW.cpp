#include <ossia/audio/audio_device.hpp>
#include <ossia/audio/audio_protocol.hpp>
#include <ossia/dataflow/graph/graph.hpp>
#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/graph/tick_methods.hpp>
#include <ossia/dataflow/graph_edge_helpers.hpp>
#include <ossia/dataflow/nodes/automation.hpp>
#include <ossia/dataflow/nodes/gain.hpp>
#include <ossia/dataflow/nodes/rand_float.hpp>
#include <ossia/dataflow/nodes/sine.hpp>
#include <ossia/detail/flicks.hpp>
#include <ossia/editor/curve/curve_segment/easing.hpp>
#include <ossia/editor/curve/curve_segment/linear.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>

struct tick
{
  ossia::execution_state& st;
  ossia::graph_interface& g;
  ossia::audio_protocol& proto;
  ossia::time_interval& itv;
  ossia::time_value prev_date{};

  void operator()(const ossia::audio_tick_state& st)
  {
    proto.setup_buffers(st);
    (*this)(st.frames, st.seconds);
  }

  void operator()(unsigned long frameCount, double seconds)
  {
    using namespace ossia;
#if defined(OSSIA_EXECUTION_LOG)
    auto log = g_exec_log.start_tick();
#endif

    std::atomic_thread_fence(std::memory_order_seq_cst);
    st.begin_tick();
    st.samples_since_start += frameCount;
    st.bufferSize = (int)frameCount;
    // we could run a syscall and call now() but that's a bit more costly.
    st.cur_date = seconds * 1e9;

    const auto flicks = frameCount * st.samplesToModelRatio;
    const ossia::token_request tok{};

    // Temporal tick
    {
#if defined(OSSIA_EXECUTION_LOG)
      auto log = g_exec_log.start_temporal();
#endif
      itv.tick_offset(ossia::time_value{int64_t(flicks)}, 0_tv, tok);
    }

    // Dataflow execution
    {
#if defined(OSSIA_EXECUTION_LOG)
      auto log = g_exec_log.start_dataflow();
#endif

      g.state(st);
    }

    std::atomic_thread_fence(std::memory_order_seq_cst);

    // Apply messages
    {
#if defined(OSSIA_EXECUTION_LOG)
      auto log = g_exec_log.start_commit();
#endif

      st.commit();
    }
  }
};

constexpr ossia::time_value seconds(double s) noexcept
{
  return ossia::time_value{int64_t(s * ossia::flicks_per_second<int64_t>)};
}

struct box
{
  ossia::time_sync& start_sync;
  ossia::time_sync& end_sync;
  ossia::time_event& start_event;
  ossia::time_event& end_event;
  ossia::time_interval& interval;
};

box create_box_after(
    ossia::scenario& scenario, ossia::time_event& start_event,
    ossia::time_value duration)
{
  auto& start_sync = start_event.get_time_sync();

  auto end_sync = std::make_shared<ossia::time_sync>();
  end_sync->set_expression(ossia::expressions::make_expression_true());

  auto end_event = *(end_sync->emplace(end_sync->get_time_events().begin(), {}));
  end_event->set_expression(ossia::expressions::make_expression_true());

  scenario.add_time_sync(end_sync);

  auto itv = ossia::time_interval::create(
      {}, start_event, *end_event, duration, duration, duration);
  scenario.add_time_interval(itv);

  return box{start_sync, *end_sync, start_event, *end_event, *itv};
}

auto create_score(ossia::graph_interface& g, ossia::audio_device& audio)
{
  using namespace ossia;
  // Create some of the dataflow nodes we are going to work with:
  auto automation_node = std::make_shared<ossia::nodes::automation>();
  {
    // Create an automation curve
    auto crv = std::make_shared<ossia::curve<double, float>>();

    // Go from 20 Hz on start
    crv->set_x0(0);
    crv->set_y0(20);

    // To 2000 Hz at 70% with an easing function
    crv->add_point(
        ossia::curve_segment_ease<float, ossia::easing::bounceInOut>(), 0.7, 2000);

    // To 200 Hz at 100%
    crv->add_point(ossia::curve_segment_linear<float>(), 1., 200);
    automation_node->set_behavior(std::move(crv));
  }

  auto rand_node = std::make_shared<ossia::nodes::rand_float>(200, 400);
  auto gain_node = std::make_shared<ossia::nodes::gain_node>();
  auto sine_node = std::make_shared<ossia::nodes::sine>();
  auto low_sine_node = std::make_shared<ossia::nodes::sine>();

  // Create the temporal structure.
  // The root of a score is always a time_interval.

  // In this interval we will create a few processes :
  // * One to hold a sine generator
  // * One to hold a scenario which will have
  //   - first an automation for 7s
  //   - nothing for 2s
  //   - and then a random generator for 5s
  // to control the frequency of the sine generator.
  // The sine wave generator's output goes through a gain node.
  // Another fixed low-frequency sine generator also outputs sound,
  // without going through any node.

  // 1st step: create that interval and the time_sync that start and stop it.
  auto main_start_node = std::make_shared<time_sync>();
  auto main_end_node = std::make_shared<time_sync>();

  auto main_start_event
      = *(main_start_node->emplace(main_start_node->get_time_events().begin(), {}));
  auto main_end_event
      = *(main_end_node->emplace(main_end_node->get_time_events().begin(), {}));

  const ossia::time_value main_duration = seconds(20);
  auto main_interval = time_interval::create(
      {}, *main_start_event, *main_end_event, main_duration, main_duration,
      main_duration);

  {
    // The scenario process is what allows a finer temporal organization of things
    std::shared_ptr<ossia::scenario> scenar = std::make_shared<ossia::scenario>();
    scenar->get_start_time_sync()->set_expression(
        ossia::expressions::make_expression_true());
    scenar->get_start_time_sync()->set_start(true);

    auto start_event = *(scenar->get_start_time_sync()->emplace(
        scenar->get_start_time_sync()->get_time_events().begin(), {}));

    // Create a first interval in this scenario, from the start to t = 5
    const auto& first_box = create_box_after(*scenar, *start_event, seconds(5));

    // Then an empty one for 1s
    const auto& second_box = create_box_after(*scenar, first_box.end_event, seconds(1));

    // And a last one for 1s
    const auto& third_box = create_box_after(*scenar, second_box.end_event, seconds(1));

    // Loop back from the end of the third box to the beginning of the second box
    auto loop_interval = time_interval::create(
        {}, third_box.end_event, second_box.start_event, 0_tv, 0_tv, ossia::Infinite);
    loop_interval->graphal = true;

    scenar->add_time_interval(loop_interval);

    // Add the processes to the boxes, the processes are what convert the temporal domain requests
    // into token requests for the dataflow graph
    auto automation_process
        = std::make_shared<ossia::nodes::automation_process>(automation_node);
    first_box.interval.add_time_process(automation_process);
    g.add_node(automation_node);

    auto random_process = std::make_shared<ossia::node_process>(rand_node);
    third_box.interval.add_time_process(random_process);
    g.add_node(rand_node);

    // Put the created scenario as a child process of the main interval
    main_interval->add_time_process(scenar);

    // In this code example we don't need to add these interval's nodes
    // as there is no hierarchic audio processing in the scenario; in
    // a more general case we would do the following to enjoy hierarchic audio mixing
    // g.add_node(scenar->node);
    // g.add_node(first_box.interval->node);
    // g.add_node(second_box.interval->node);
    // g.add_node(third_box.interval->node);
    // and also add the relevant connections to the graph
  }

  // Do the same for the other processes in our main interval
  auto sine_process = std::make_shared<ossia::node_process>(sine_node);
  main_interval->add_time_process(sine_process);
  g.add_node(sine_node);

  auto low_sine_process = std::make_shared<ossia::node_process>(low_sine_node);
  main_interval->add_time_process(low_sine_process);
  g.add_node(low_sine_node);

  auto gain_process = std::make_shared<ossia::node_process>(gain_node);
  main_interval->add_time_process(gain_process);
  g.add_node(gain_node);

  // Create the connections
  g.connect(make_glutton_edge(g, 0, 0, automation_node, sine_node));
  g.connect(make_glutton_edge(g, 0, 0, rand_node, sine_node));
  g.connect(make_glutton_edge(g, 0, 0, sine_node, gain_node));
  g.connect(make_glutton_edge(g, 0, 0, gain_node, main_interval->node));
  g.connect(make_glutton_edge(g, 0, 0, low_sine_node, main_interval->node));

  // Easy shortcut for the 1/2 outputs on your soundcard
  assert(main_interval);
  assert(main_interval->node);
  assert(main_interval->node->root_outputs().size() > 0);
  assert(main_interval->node->root_outputs()[0]);
  main_interval->node->root_outputs()[0]->address = &audio.get_main_out();

  // Set some parameters
  gain_node->gain = 0.5;
  low_sine_node->freq = 100;
  low_sine_node->amplitude = 0.65;

  return main_interval;
}

int main(int argc, char** argv)
{
  using namespace std::literals;

  // Setup our datastructures
  auto graph = std::make_unique<ossia::tc_graph>();
  ossia::audio_device audio{
      "minidaw", // name, for APIs like JACK
      256,       // buffer size
      48000,     // sample rate
      0, 2       // inputs, outputs
  };

  ossia::execution_state e;

  e.sampleRate = audio.get_sample_rate();
  e.bufferSize = audio.get_buffer_size();

  e.modelToSamplesRatio = audio.get_sample_rate() / ossia::flicks_per_second<double>;
  e.samplesToModelRatio = ossia::flicks_per_second<double> / audio.get_sample_rate();
  e.register_device(&audio.device);

  auto root_interval = create_score(*graph, audio);
  root_interval->start();

  audio.engine->set_tick(tick{e, *graph, audio.protocol, *root_interval});

  std::this_thread::sleep_for(20s);

  audio.engine->stop();
}
