#include <ossia/dataflow/graph/graph.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/editor/scenario/time_interval.hpp>
#include <ossia/editor/scenario/scenario.hpp>
#include <ossia/editor/scenario/time_event.hpp>
#include <ossia/editor/scenario/time_sync.hpp>
#include <ossia/editor/scenario/clock.hpp>

/** utility functions to move somewhere else **/
struct root_scenario
{
  std::shared_ptr<ossia::time_sync> start_node{std::make_shared<ossia::time_sync>()};
  std::shared_ptr<ossia::time_sync> end_node{std::make_shared<ossia::time_sync>()};

  std::shared_ptr<ossia::time_event> start_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *start_node, ossia::expressions::make_expression_true())};
  std::shared_ptr<ossia::time_event> end_event{std::make_shared<ossia::time_event>(ossia::time_event::exec_callback{}, *end_node, ossia::expressions::make_expression_true())};

  std::shared_ptr<ossia::time_interval> interval;
  std::shared_ptr<ossia::scenario> scenario;

  std::shared_ptr<ossia::time_event> scenario_start;
  root_scenario(ossia::time_value dur):
    interval{ossia::time_interval::create(
               [] (auto&&...) {},
               *start_event, *end_event, dur, dur, dur)}
  , scenario{std::make_shared<ossia::scenario>()}
  {
    using namespace ossia;
    start_node->insert(start_node->get_time_events().end(), start_event);
    end_node->insert(end_node->get_time_events().end(), end_event);

    interval->add_time_process(this->scenario);
    auto scen_sn = this->scenario->get_start_time_sync();
    auto scen_se = std::make_shared<ossia::time_event>(
          ossia::time_event::exec_callback{},
          *scen_sn,
          ossia::expressions::make_expression_true());
    scen_sn->insert(scen_sn->get_time_events().end(), scen_se);
    scenario_start = scen_se;
  }

  auto& add_event() {
    auto tn = std::make_shared<ossia::time_sync>();
    scenario->add_time_sync(tn);
    auto ev = std::make_shared<ossia::time_event>(
          ossia::time_event::exec_callback{},
          *tn,
          ossia::expressions::make_expression_true());
    tn->insert(tn->get_time_events().end(), ev);
    return *ev;
  }

  auto& add_interval(ossia::time_value tv, ossia::time_event& sev, ossia::time_event& eev)
  {
    auto itv = ossia::time_interval::create(ossia::time_interval::exec_callback{}, sev, eev, tv, tv, tv);
    scenario->add_time_interval(itv);
    return *itv;
  }
};


template<typename T>
ossia::optional<T> pop_value(const ossia::inlet_ptr& p) {
  if(p)
  {
    auto ip = p->data.target<ossia::value_port>();
    if(ip)
    {
      if(!ip->get_data().empty())
      {
        if(auto res = ip->get_data().front().value.target<T>())
        {
          return *res;
        }
      }
    }
  }
  return ossia::none;
}

void push_value(const ossia::outlet_ptr& p, ossia::value val) {
  if(p)
  {
    if(auto op = p->data.target<ossia::value_port>())
      op->add_raw_value(std::move(val));
  }
}

struct my_node : ossia::graph_node {
    my_node() {
      inputs().push_back(ossia::make_inlet<ossia::value_port>());
      outputs().push_back(ossia::make_outlet<ossia::value_port>());
    }

    void run(ossia::token_request t, ossia::execution_state&) override {
      if(auto a_float = pop_value<float>(this->inputs()[0])) {
        push_value(this->outputs()[0], 100 + 50 * std::cos(*a_float) * std::sin(10. * t.position));
      }
    }
};
int main()
{
  using namespace ossia;
  using namespace ossia::net;
  using namespace std::literals;

  generic_device device(std::make_unique<oscquery::oscquery_mirror_protocol>("ws://127.0.0.1:5678"), "max");

  device.get_protocol().update(device.get_root_node());
  auto foo_p = ossia::net::find_node(device.get_root_node(), "/saw");
  auto bar_p = ossia::net::find_node(device.get_root_node(), "/phasor");

  if(!foo_p || !bar_p)
    return 1;

  auto foo = foo_p->get_parameter();
  auto bar = bar_p->get_parameter();
  /* here we use the Max-created environment instead
  // Create an environment
  auto& root = device.get_root_node();

  auto foo = create_node(root, "/foo").create_parameter(val_type::FLOAT);
  auto bar = create_node(root, "/bar").create_parameter(val_type::FLOAT);

  foo->push_value(100.0);
  */
  foo->push_value(100.0);
  bar->push_value(1.0);

  // Create nodes
  auto g = std::make_shared<graph>();
  auto node1 = std::make_shared<my_node>();
  node1->inputs()[0]->address = foo;
  node1->outputs()[0]->address = foo;
  g->add_node(node1);

  auto node2 = std::make_shared<my_node>();
  node2->inputs()[0]->address = foo;
  node2->outputs()[0]->address = bar;
  g->add_node(node2);

  // Create a 5 second score
  root_scenario score{15000_tv};
  // A branch lasts 3 seconds
  auto& itv1 = score.add_interval(7000_tv, *score.scenario_start, score.add_event());

  // Another lasts 4
  auto& mid_ev = score.add_event();
  score.add_interval(4000_tv, *score.scenario_start, mid_ev);
  auto& itv3 = score.add_interval(9000_tv, mid_ev, score.add_event());

  itv1.add_time_process(std::make_shared<ossia::node_process>(node1));
  itv3.add_time_process(std::make_shared<ossia::node_process>(node2));

  // What do we do at each tick
  score.interval->set_callback([&] (auto&&...) {
    ossia::execution_state e;
    g->state(e);
    e.commit();
    std::cerr << "/foo: " << foo->value() << "\t";
    std::cerr << "/bar: " << bar->value() << "\n";
  });

  // Start execution in a thread
  ossia::clock clck{*score.interval};
  clck.set_granularity(100ms);
  clck.start_and_tick();

  while(clck.running()) ;
}
