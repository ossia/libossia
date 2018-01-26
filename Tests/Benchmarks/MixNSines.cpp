#include <ossia/dataflow/graph/graph_static.hpp>
#include <ossia/dataflow/audio_protocol.hpp>
#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/dataflow/common_nodes.hpp>
#include <valgrind/callgrind.h>
#include "../Editor/TestUtils.hpp"
#include <cmath>


static const constexpr int NUM_TAKES = 10;
static const constexpr auto NUM_CURVES = { 1, 10, 20, 30, 40,
                                                                                     50, 60, 70, 80, 90,
                                                                                     100, 150, 200, 250,
                                                                                     300, 400, 500
                                                                                     , 600, 700, 800, 900, 1000
                                         };
// Mix N sines with addresses as inputs

int main()
{
  using namespace ossia;
  // Benchmark: how many automations can run at the same time
  // We need a graph


  std::cout << "count\tnormal\tordered\tmerged\n";
  for(int N : NUM_CURVES)
  {
    std::vector<double> counts(3);
    for(int i = 0; i < NUM_TAKES; i++)
    {
      audio_device device;
      tc_graph g;
      std::vector<ossia::node_ptr> nodes;

      auto gain = std::make_shared<gain_node>();
      g.add_node(gain);
      nodes.push_back(gain);
      gain->outputs()[0]->address = &device.get_main_out();

      for(int i = 0; i < 60; i++)
      {
        auto node = std::make_shared<sine_node>();
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
      ossia::time_value v{};
      // run a first tick to init the graph

      ossia::time_value cur_time{};
      e.clear_local_state();
      e.get_new_values();
      for(auto& node : nodes) { node->requested_tokens.push_back({cur_time}); }
      g.state(e);
      e.commit();

      int k = 0;
      for(auto fun : {&execution_state::commit, &execution_state::commit_ordered, &execution_state::commit_merged})
      {
        int64_t count = 0;

        auto t0 = std::chrono::steady_clock::now();
        CALLGRIND_START_INSTRUMENTATION;
        e.clear_local_state();
        e.get_new_values();
        cur_time += 64;
        for(auto& node : nodes) { node->requested_tokens.push_back(ossia::token_request{cur_time}); }
        g.state(e);
        (e.*fun)();
        CALLGRIND_STOP_INSTRUMENTATION;
        auto t1 = std::chrono::steady_clock::now();
        count += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        v = v + (int64_t)1;

        counts[k++] += count;
      }
    }

    std::cout << N
              << "\t" << counts[0]
              << "\t" << counts[1]
              << "\t" << counts[2]
              << std::endl;
  }
  CALLGRIND_DUMP_STATS;
}
