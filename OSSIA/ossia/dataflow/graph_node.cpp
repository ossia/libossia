// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/dataflow/fx_node.hpp>

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

std::string_view graph_node::label() const
{
  return {};
}


void graph_node::clear()
{
  m_inlets.clear();
  m_outlets.clear();
}
}
