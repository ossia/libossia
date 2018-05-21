#include <ossia/dataflow/graph/graph_static.hpp>

#include <ossia/editor/automation/automation.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <valgrind/callgrind.h>
#include <random>
#include "../Editor/TestUtils.hpp"


static const constexpr int NUM_TAKES = 5;
static const constexpr auto NUM_CURVES = { 10, 20, 30, 40,
                                           50, 60, 70, 80, 90,
                                           100, 150, 200, 250,
                                           300, 400, 500
                                           , 600, 700, 800, 900, 1000};

static const constexpr auto CABLE_PROBABILITY = { 0.00001, 0.0001, 0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007, 0.008, 0.009, 0.01/*, 0.02, 0.03 */};

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
  std::random_device dev;
  std::mt19937 mt(dev());
  using namespace ossia;
  // Benchmark: how many automations can run at the same time
  // We need a graph


  for(double D : CABLE_PROBABILITY)
  {
    std::cout << "Density: " << D << std::endl;
    std::cout << "nodes\tcables\tmessages\tnormal\tordered\tmerged\n";
    for(int N : NUM_CURVES)
    {
      std::vector<double> counts(3);
      double avg_num_cables{};
      double avg_msg_count{};
      bool must_break = false;
      for(int i = 0; i < NUM_TAKES; i++)
      {
        TestDevice t;
        tc_graph g;

        std::vector<ossia::node_ptr> automs;
        std::vector<ossia::node_ptr> mappings;
        for(int i = 0; i < N; i++)
        {
          if(i%2)
          {
            auto node = std::make_shared<automation_node>();

            auto v = std::make_shared<ossia::curve<double, float>>();
            v->set_x0(0.); v->set_y0(0.);
            v->add_point(ossia::easing::ease{}, 1., 1.);
            node->set_behavior(v);
            automs.push_back(node);
            g.add_node(std::move(node));
          }
          else
          {
            auto node = std::make_shared<mapping_node>();

            auto v = std::make_shared<ossia::curve<float, float>>();
            v->set_x0(0.); v->set_y0(0.);
            v->add_point(ossia::easing::ease{}, 1., 1.);
            node->set_behavior(v);
            node->set_driven(destination{*t.float_params[std::abs(rand()) % t.float_params.size()]});
            mappings.push_back(node);
            g.add_node(std::move(node));
          }
        }

        if(automs.size() < 2 || mappings.size() < 2 || automs.size() != mappings.size())
          return 1;

        // Create some cables
        using bdist = std::bernoulli_distribution;
        int num_cables = 0;
        for(std::size_t i = 0; i < mappings.size(); i++)
        {
          const auto& autom = automs[i];
          const auto& mapping = mappings[i];

          g.connect(ossia::make_edge(ossia::immediate_strict_connection{}, autom->outputs()[0], mapping->inputs()[0], autom, mapping));
          num_cables++;
        }

        for(std::size_t i = 0; i < mappings.size(); i++)
        {
          for(std::size_t j = i + 1; j < mappings.size(); j++)
          {
            const auto& source = mappings[i];
            const auto& target = mappings[j];
            if(bdist{D}(mt))
            {
              g.connect(ossia::make_edge(ossia::immediate_strict_connection{}, source->outputs()[0], target->inputs()[0], source, target));

              num_cables++;
            }
          }
        }

        for(auto mapping : mappings)
        {
          if(!mapping->outputs()[0]->targets.empty())
            mapping->outputs()[0]->address = {};
        }

        //ossia::print_graph(g.impl(), std::cerr);
        ossia::execution_state e;
        e.register_device(&t.device);
        ossia::time_value v{};
        // run a first tick to init the graph


        e.clear_local_state();
        e.get_new_values();
        for(auto& n : g.m_nodes)
          n.first->request(ossia::token_request{ossia::time_value{1}});

        g.state(e);
        std::size_t msg_count = num_messages(e);
        if(msg_count > 50000)
        {
          //std::cout << N << "\t" << num_cables << "\t" << msg_count << "\t" << "nan" << "\t" << "nan" << "\t" << "nan" << std::endl;
          must_break = true;
          break;
        }
        avg_msg_count += msg_count;
        avg_num_cables += num_cables;
        e.commit();


        int k = 0;
        for(auto fun : {&execution_state::commit, &execution_state::commit_ordered, &execution_state::commit_merged})
        {
          int64_t count = 0;
          for(auto& n : g.m_nodes)
            n.first->request(ossia::token_request{ossia::time_value{k}});

          auto t0 = std::chrono::steady_clock::now();
          CALLGRIND_START_INSTRUMENTATION;
          e.clear_local_state();
          e.get_new_values();
          g.state(e);
          (e.*fun)();
          CALLGRIND_STOP_INSTRUMENTATION;
          auto t1 = std::chrono::steady_clock::now();
          auto t = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
          if(t > 5000)
          {
            must_break = true;
            break;
          }
          count += t;
          v = v + (int64_t)1;

          counts[k++] += count;
        }
        if(must_break)
          break;
      }
      if(must_break)
        break;
      std::cout << N
                << "\t" << avg_num_cables / double(NUM_TAKES)
                << "\t" << avg_msg_count / double(NUM_TAKES)
                << "\t" << counts[0] / double(NUM_TAKES)
                << "\t" << counts[1] / double(NUM_TAKES) << "\t"
                << counts[2] / double(NUM_TAKES) << std::endl;

    }
  }
  CALLGRIND_DUMP_STATS;
}
