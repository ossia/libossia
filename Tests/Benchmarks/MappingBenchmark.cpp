#include <ossia/dataflow/graph/graph_static.hpp>

#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <valgrind/callgrind.h>
#include "../Editor/TestUtils.hpp"


static const constexpr int NUM_TAKES = 10;
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


  std::cout << "count\tmessages\tnormal\tordered\tmerged\n";
  for(int N : NUM_CURVES)
  {
    std::vector<double> counts(3);
    double avg_msg_count{};
    for(int i = 0; i < NUM_TAKES; i++)
    {
      TestDevice t;
      tc_graph g;
      scenario s;
      g.add_node(s.node);

      auto sev = *s.get_start_time_sync()->emplace(s.get_start_time_sync()->get_time_events().end(), {}, {});
      for(int i = 0; i < N; i++)
      {
        std::shared_ptr<time_sync> tn = std::make_shared<time_sync>();
        s.add_time_sync(tn);
        auto ev = *tn->emplace(tn->get_time_events().end(), {}, {});

        auto tc = time_interval::create({}, *sev, *ev, 0_tv, 1000_tv, ossia::Infinite);
        s.add_time_interval(tc);
        g.add_node(tc->node);

        if(i%2)
        {
          auto node = std::make_shared<automation_node>();
          auto autom = std::make_shared<automation_process>(node);
          node->set_destination(destination{*t.float_params[std::abs(rand()) % t.float_params.size()]});

          auto v = std::make_shared<ossia::curve<double, float>>();
          v->set_x0(0.); v->set_y0(0.);
          v->add_point(ossia::easing::ease{}, 1., 1.);
          node->set_behavior(v);
          autom->node = node;
          tc->add_time_process(autom);
          g.add_node(node);
        }
        else
        {
          auto node = std::make_shared<mapping_node>();
          auto autom = std::make_shared<node_process>(node);
          node->set_driver(destination{*t.float_params[std::abs(rand()) % t.float_params.size()]});
          node->set_driven(destination{*t.float_params[std::abs(rand()) % t.float_params.size()]});

          auto v = std::make_shared<ossia::curve<float, float>>();
          v->set_x0(0.); v->set_y0(0.);
          v->add_point(ossia::easing::ease{}, 1., 1.);
          node->set_behavior(v);
          autom->node = node;
          tc->add_time_process(autom);
          g.add_node(node);
        }
      }

      ossia::execution_state e;
      e.register_device(&t.device);
      ossia::time_value v{};
      s.start();
      // run a first tick to init the graph

      e.clear_local_state();
      e.get_new_values();
      s.state(v, 0., 0_tv, 0_tv);
      g.state(e);
      std::size_t msg_count = num_messages(e);
      avg_msg_count += msg_count;
      e.commit();

      int k = 0;
      for(auto fun : {&execution_state::commit, &execution_state::commit_ordered, &execution_state::commit_merged})
      {
        int64_t count = 0;

        auto t0 = std::chrono::steady_clock::now();
        CALLGRIND_START_INSTRUMENTATION;
        e.clear_local_state();
        e.get_new_values();
        s.state(v, 0., 0_tv, 0_tv);
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
