
#include <flat_hash_map.hpp>
#include "../Editor/TestUtils.hpp"
#include <ossia/detail/pod_vector.hpp>
#include <valgrind/callgrind.h>
#include <random>

#define private public
#include <ossia/dataflow/nodes/automation.hpp>
#include <ossia/dataflow/nodes/mapping.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/dataflow/graph/graph_static.hpp>


static const constexpr int NUM_TAKES = 500;
static const constexpr auto NUM_CURVES = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41
                                           /*1, 10, 20, 30, 40
                                                                                     ,
                                                                                     50, 60, 70, 80, 90,
                                                                                     100, 150, 200, 250,
                                                                                     300, 400, 500
                                                                                     , 600, 700, 800, 900, 1000*/
                                         };


std::size_t num_messages(ossia::execution_state& e)
{
  std::size_t count = 0;
  for(auto& mq : e.m_valueState)
  {
    count += mq.second.size();

  }
  return count;
}
int main()
{
  using namespace ossia;
  // Benchmark: how many automations can run at the same time
  // We need a graph

  std::mt19937 rand;
  std::cout << "count\tmessages\tnormal\tordered\tmerged\n";
  for(int N : NUM_CURVES)
  {
    ossia::double_vector counts(3);
    double avg_msg_count{};
    for(int i = 0; i < NUM_TAKES; i++)
    {
      TestDevice t;
      std::uniform_int_distribution<int> dist{(int)0, (int)t.float_params.size() - 1};
      tc_graph g;
      for(int i = 0; i < N; i++)
      {
        if(i%2)
        {
          auto node = std::make_shared<ossia::nodes::automation>();
          node->outputs()[0]->address = t.float_params[dist(rand)];

          auto v = std::make_shared<ossia::curve<double, float>>();
          v->set_x0(0.); v->set_y0(0.);
          v->add_point(ossia::easing::ease{}, 1., 1.);
          node->set_behavior(v);
          g.add_node(node);
        }
        else
        {
          auto node = std::make_shared<ossia::nodes::mapping>();
          node->value_in.address = t.float_params[dist(rand)];
          node->value_out.address = t.float_params[dist(rand)];

          auto v = std::make_shared<ossia::curve<float, float>>();
          v->set_x0(0.); v->set_y0(0.);
          v->add_point(ossia::easing::ease{}, 1., 1.);
          node->set_behavior(v);
          g.add_node(node);
        }
      }

      ossia::execution_state e;
      e.register_device(&t.device);
      ossia::time_value v{};
      // run a first tick to init the graph

      e.clear_local_state();
      e.get_new_values();
      for(auto& n : g.m_nodes)
        n.first->request(ossia::token_request{0_tv, 1_tv, 0., 0_tv});

      g.state(e);
      std::size_t msg_count = num_messages(e);
      avg_msg_count += msg_count;
      e.commit();

      int k = 0;

      for(auto fun : {&execution_state::commit, &execution_state::commit_ordered, &execution_state::commit_merged})
      {
        int64_t count = 0;
        for(auto& n : g.m_nodes)
          n.first->request(ossia::token_request{0_tv, 1_tv, 0., 0_tv});

        auto t0 = std::chrono::steady_clock::now();
        CALLGRIND_START_INSTRUMENTATION;
        e.clear_local_state();
        e.get_new_values();
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
              << "\t" << avg_msg_count / double(NUM_TAKES)
              << "\t" << counts[0] / double(NUM_TAKES)
              << "\t" << counts[1] / double(NUM_TAKES)
              << "\t" << counts[2] / double(NUM_TAKES)
              << std::endl;
  }
  CALLGRIND_DUMP_STATS;
}
