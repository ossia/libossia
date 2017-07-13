#include <ossia/dataflow/graph_node.hpp>

namespace ossia
{

graph_node::~graph_node()
{
  // TODO moveme in cpp
}

graph_node::graph_node()
{
}

bool graph_node::enabled() const
{
  return m_enabled;
}

void graph_node::set_enabled(bool b)
{
  m_enabled = b;
}

bool graph_node::executed() const
{
  return m_executed;
}

void graph_node::set_executed(bool b)
{
  m_executed = b;
}

bool graph_node::consumes(const std::string&) const
{
  return false;
}

bool graph_node::consumes(const Destination&) const
{
  return false;
}

bool graph_node::consumes(const execution_state&) const
{
  return false;
}

void graph_node::run(execution_state&)
{
}

void graph_node::set_date(ossia::time_value d)
{
  m_prev_date = m_date;
  m_date = d;
}

bool graph_node::can_execute(const execution_state&) const
{
  return ossia::all_of(m_inlets, [](const auto& inlet) {
    // A port can execute if : - it has source ports and all its source ports
    // have executed
    bool previous_nodes_executed
        = ossia::all_of(inlet->sources, [&](graph_edge* edge) {
            return edge->out_node->executed()
                   || (!edge->out_node->enabled() && bool(inlet->address)
                       /* TODO check that it's in scope */);
          });

    // it does not have source ports ; we have to check for variables :
    // find if address available in local / global scope
    return !inlet->sources.empty() ? previous_nodes_executed : true // TODO
        ;
  });
}

bool graph_node::has_port_inputs() const
{
  return ossia::any_of(
      inputs(), [](const auto& inlet) { return !inlet->sources.empty(); });
}

bool graph_node::has_global_inputs() const
{
  return ossia::any_of(inputs(), [&](const auto& inlet) {
    return (inlet->scope & port::scope_t::global) && bool(inlet->address);
  });
}

bool graph_node::has_local_inputs(const execution_state& st) const
{
  return ossia::any_of(inputs(), [&](const inlet_ptr& inlet) {
    if (inlet->scope & port::scope_t::local)
    {
      if (auto dest = inlet->address.target<ossia::net::address_base*>())
      {
        if (st.in_local_scope(**dest))
          return true;
      }
      // else if(auto pattern = pt.target<std::string>())
      // {
      // what happens if a pattern matches anotehr pattern. c.f. notes
      //   if( n.consumes(*pattern))
      //     return true;
      // }

      if (consumes(st))
        return true;
    }
    return false;
  });
}

const inlets& graph_node::inputs() const
{
  return m_inlets;
}

const outlets& graph_node::outputs() const
{
  return m_outlets;
}

void graph_node::clear()
{
  m_inlets.clear();
  m_outlets.clear();
}
}
