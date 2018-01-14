#pragma once
#include <ossia/detail/apply.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/dataflow/execution_state.hpp>

namespace ossia {
namespace detail
{
struct state_exec_visitor
{
    ossia::execution_state& e;
    void operator()(const ossia::state& st)
    {

    }

    void operator()(const ossia::message& msg)
    {
      e.insert(msg.dest.address(), {ossia::tvalue{ msg.message_value, msg.dest.index, msg.dest.unit }});
    }

    template<std::size_t N>
    void operator()(const ossia::piecewise_vec_message<N>& st)
    {
    }

    void operator()(const ossia::piecewise_message& st)
    {
    }

    void operator()()
    {

    }
};
}
struct state_node : public ossia::graph_node
{
  public:
    state_node(const ossia::state& other): data{other} { }
    state_node(ossia::state&& other): data{std::move(other)} { }
    state_node(): data{} { }

    ~state_node() override
    {

    }

    void run(ossia::token_request, ossia::execution_state& e) override
    {
      for(auto& msg : data)
      {
        ossia::apply(detail::state_exec_visitor{e}, msg);
      }
    }

    ossia::state data;
};

}

