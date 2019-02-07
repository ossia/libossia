#include <ossia/detail/hash_map.hpp>
#include <ossia/dataflow/nodes/gain.hpp>
#include <ossia/dataflow/nodes/sine.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <valgrind/callgrind.h>
#include <cmath>
#include <ossia/detail/pod_vector.hpp>
#include <boost/graph/adjacency_list.hpp>
#define private public
#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/audio/audio_protocol.hpp>
#include <ossia/dataflow/nodes/automation.hpp>
#include "../Editor/TestUtils.hpp"


static const constexpr int NUM_TAKES = 10;
static const constexpr auto NUM_CURVES = { 1, 10, 20, 30, 40,
                                           50, 60, 70, 80, 90,
                                           100, 150, 200, 250,
                                           300, 400, 500,
                                           600, 700, 800, 900, 1000
                                         };
// Mix N sines with addresses as inputs

void benchmark_main()
{
  using namespace ossia;
  // Benchmark: how many automations can run at the same time
  // We need a graph


  std::cout << "count\tnormal\tordered\tmerged\n";
  audio_device device;
  int64_t count = 0;

  int k = 0;
  for(int N : NUM_CURVES)
  {
    ossia::double_vector counts(3);
    for(int i = 0; i < NUM_TAKES; i++)
    {
      tc_graph g;
      std::vector<ossia::node_ptr> nodes;

      auto gain = std::make_shared<ossia::nodes::gain>();
      g.add_node(gain);
      nodes.push_back(gain);
      gain->outputs()[0]->address = &device.get_main_out();

      for(int i = 0; i < N; i++)
      {
        auto node = std::make_shared<ossia::nodes::sine>();
        g.add_node(node);
        g.connect(
              make_edge(
                immediate_strict_connection{},
                node->outputs()[0], gain->inputs()[0],
                node, gain));
        nodes.push_back(node);
      }

      execution_state e;
      e.sampleRate = 44100;
      e.register_device(&device.device);
      // run a first tick to init the graph

      ossia::time_value cur_time{};
      e.clear_local_state();
      e.get_new_values();
      ossia::token_request tk{0_tv, 0_tv, 0., 0_tv};
      for(auto& node : nodes) { node->request(tk); }
      g.state(e);
      e.commit();

      for(auto fun : {&execution_state::commit, &execution_state::commit_ordered, &execution_state::commit_merged})
      {
        auto t0 = std::chrono::steady_clock::now();
        e.clear_local_state();
        e.get_new_values();
        ossia::token_request tk{cur_time, cur_time + 64_tv, 0., 0_tv};
        for(auto& node : nodes)
        {
          node->request(tk);
        }
        cur_time += 64;
        g.state(e);
        (e.*fun)();
        auto t1 = std::chrono::steady_clock::now();

        count += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        k++;
      }
    }
  }

  std::cout << "Total: " << double(count) / double(k);

}

int main() { benchmark_main(); }
