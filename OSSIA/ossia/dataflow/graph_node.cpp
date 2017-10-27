// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/dataflow.hpp>


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
  return !requested_tokens.empty();
}

void graph_node::disable()
{
  requested_tokens.clear();
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

bool graph_node::consumes(const destination&) const
{
  return false;
}

bool graph_node::consumes(const execution_state&) const
{
  return false;
}

void graph_node::run(token_request t, execution_state&)
{
}

bool graph_node::can_execute(const execution_state&) const
{
  return ossia::all_of(m_inlets, [](const auto& inlet) {
    // A port can execute if : - it has source ports and all its source ports
    // have executed
    bool previous_nodes_executed
        = ossia::all_of(inlet->sources, [&](graph_edge* edge) {
            return edge->out_node->executed()
                   || (!edge->out_node->enabled() /* && bool(inlet->address) */
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
      inputs(), [](const inlet_ptr& inlet) { return !inlet->sources.empty(); });
}

bool graph_node::has_global_inputs() const
{
  return ossia::any_of(inputs(), [&](const inlet_ptr& inlet) {
    return (inlet->scope & port::scope_t::global) && bool(inlet->address);
  });
}

bool graph_node::has_local_inputs(const execution_state& st) const
{
  return ossia::any_of(inputs(), [&] (const inlet_ptr& inlet) {
    if (inlet->scope & port::scope_t::local)
    {
      bool b = false;

      // TODO optimize by stopping when found
      apply_to_destination(inlet->address, st,
                           [&] (ossia::net::parameter_base* addr) {
        if (!b || st.in_local_scope(*addr))
          b = true;
      });

      if(b)
        return true;

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

inlets& graph_node::inputs()
{
  return m_inlets;
}

outlets& graph_node::outputs()
{
  return m_outlets;
}

void graph_node::clear()
{
  m_inlets.clear();
  m_outlets.clear();
}
}
