#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/graph/tick_methods.hpp>
#include <ossia/audio/audio_protocol.hpp>
#include <ossia/dataflow/nodes/rand_float.hpp>
#include <ossia/dataflow/nodes/sine.hpp>
#include <ossia/dataflow/nodes/gain.hpp>


#if __has_include(<valgrind/callgrind.h>)
#include <valgrind/callgrind.h>
#endif
struct tick_all_nodes_bench
{
    ossia::execution_state& e;
    ossia::graph_interface& g;

    void operator()(unsigned long samples, double) const
    {
#if __has_include(<valgrind/callgrind.h>)
      CALLGRIND_START_INSTRUMENTATION;
#endif
      e.begin_tick();
      ossia::time_value old_t{e.samples_since_start};
      e.samples_since_start += samples;
      ossia::time_value new_t{e.samples_since_start};

      for(auto& node : g.get_nodes())
        node->request(ossia::token_request{old_t, new_t});

      g.state(e);
      e.commit();
#if __has_include(<valgrind/callgrind.h>)
      CALLGRIND_STOP_INSTRUMENTATION;
#endif
    }
};

int main(int argc, char** argv)
{
  int nodes = 60;
  if(argc > 1)
    nodes = std::atoi(argv[1]);
  using namespace ossia;
  using namespace std::literals;

  tc_graph g;
  execution_state e;
  audio_device audio;

  auto gain = std::make_shared<ossia::nodes::gain>();
  g.add_node(gain);
  gain->outputs()[0]->address = &audio.get_main_out();

  for(int i = 0; i < nodes; i++)
  {
    auto rand_node = std::make_shared<ossia::nodes::rand_float>(200, 600);
    auto sin_node = std::make_shared<ossia::nodes::sine>();
    g.connect(make_strict_edge(0, 0, rand_node, sin_node));
    g.connect(make_strict_edge(0, 0, sin_node, gain));
  }

  e.sampleRate = audio.get_sample_rate();
  e.bufferSize = audio.get_buffer_size();
  e.register_device(&audio.device);

  g.state(e);
  audio.protocol.set_tick(tick_all_nodes{e, g});

  std::this_thread::sleep_for(10s);
}
