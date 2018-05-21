#include <ossia/dataflow/graph/graph_static.hpp>

#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <valgrind/callgrind.h>
#include "../Editor/TestUtils.hpp"
#include <ossia/dataflow/graph/graph_utils.hpp>



static const constexpr int NUM_TAKES = 100;
static const constexpr auto NUM_CURVES = {1, 10, 20, 30, 40,
                                          50, 60, 70, 80, 90,
                                          100, 150, 200, 250,
                                          300, 400, 500
                                          , 600, 700, 800, 900, 1000};


struct tick_nodes_custom
{
    ossia::execution_state& e;
    ossia::graph_base& g;

    template<typename Fun>
    void operator()(unsigned long samples, Fun f) const
    {
      using namespace ossia;
      e.clear_local_state();
      e.get_new_values();
      e.samples_since_start += samples;

      for(auto& node : g.m_nodes)
        node.first->request(token_request{time_value{e.samples_since_start}});

      g.state(e);
      (e.*f)();
    }
};
int main()
{
  using namespace ossia;
  // Benchmark: how many automations can run at the same time
  // We need a graph

  std::cout << "count\tnormal\tordered\tmerged\n";
  for(int N : NUM_CURVES)
  {
    TestDevice t;
    tc_graph g;

    for(int i = 0; i < N; i++)
    {
      auto node = std::make_shared<automation_node>();
      node->set_destination(destination{*t.float_params[std::abs(rand()) % t.float_params.size()]});

      auto v = std::make_shared<ossia::curve<double, float>>();
      v->set_x0(0.); v->set_y0(0.);
      v->add_point(ossia::easing::ease{}, 1., 1.);
      node->set_behavior(v);
      g.add_node(node);
    }

    ossia::execution_state e;
    e.register_device(&t.device);
    tick_nodes_custom tick{e,g};
    ossia::time_value v{};
    // run a first tick to init the graph

    int i = 1;
    tick(i++, &execution_state::commit);
    tick(i++, &execution_state::commit_ordered);
    tick(i++, &execution_state::commit_merged);
    std::vector<double> counts;
    for(auto fun : {&execution_state::commit, &execution_state::commit_ordered, &execution_state::commit_merged})
    {
      int64_t count = 0;

      for(int i = 0; i < NUM_TAKES; i++)
      {
        auto t0 = std::chrono::steady_clock::now();
        CALLGRIND_START_INSTRUMENTATION;
        tick(i++, fun);
        CALLGRIND_STOP_INSTRUMENTATION;
        auto t1 = std::chrono::steady_clock::now();
        count += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        v = v + (int64_t)1;
      }
      counts.push_back(count / double(NUM_TAKES));
    }

    std::cout << N << "\t" << counts[0] << "\t" << counts[1] << "\t" << counts[2] << std::endl;
  }
  CALLGRIND_DUMP_STATS;
}
