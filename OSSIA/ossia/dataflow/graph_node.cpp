// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/fx_node.hpp>
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/dataflow.hpp>

namespace ossia
{

graph_node::~graph_node()
{
  // TODO moveme in cpp
}
audio_fx_node::~audio_fx_node()
{

}
graph_node::graph_node()
{
}

bool graph_node::consumes(const execution_state&) const
{
  return false;
}

void graph_node::run(token_request t, execution_state&)
{
}

std::string graph_node::label() const
{
  return {};
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
      // TODO valueDevices -> generalize
      apply_to_destination(inlet->address, st.allDevices,
                           [&] (ossia::net::parameter_base* addr, bool) {
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


void graph_node::clear()
{
  m_inlets.clear();
  m_outlets.clear();
}
}
