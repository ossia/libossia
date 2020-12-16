#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>
namespace ossia
{
template<typename Fin>
void for_each_inlet(const ossia::graph_node& node, Fin&& fin) noexcept
{
  for(auto port : node.root_inputs())
  {
    fin(*port);
    for(auto sub : port->child_inlets)
    {
      fin(*sub);
    }
  }
  for(auto port : node.root_outputs())
  {
    for(auto sub : port->child_inlets)
    {
      fin(*sub);
    }
  }
}

template<typename Fin>
bool any_of_inlet(const ossia::graph_node& node, Fin&& fin) noexcept
{
  for(auto port : node.root_inputs())
  {
    if(fin(*port))
      return true;

    for(auto sub : port->child_inlets)
    {
      if(fin(*sub))
        return true;
    }
  }
  for(auto port : node.root_outputs())
  {
    for(auto sub : port->child_inlets)
    {
      if(fin(*sub))
        return true;
    }
  }
  return false;
}

template<typename Fin>
bool all_of_inlet(const ossia::graph_node& node, Fin&& fin) noexcept
{
  for(auto port : node.root_inputs())
  {
    if(!fin(*port))
      return false;

    for(auto sub : port->child_inlets)
    {
      if(!fin(*sub))
        return false;
    }
  }
  for(auto port : node.root_outputs())
  {
    for(auto sub : port->child_inlets)
    {
      if(!fin(*sub))
        return false;
    }
  }
  return true;
}

template<typename Fout>
void for_each_outlet(const ossia::graph_node& node, Fout&& fout) noexcept
{
  for(auto port : node.root_outputs())
  {
    fout(*port);
  }
}

template<typename Fout>
bool any_of_outlet(const ossia::graph_node& node, Fout&& fout) noexcept
{
  for(auto port : node.root_outputs())
  {
    if(fout(*port))
      return true;
  }
  return false;
}

template<typename Fout>
bool all_of_outlet(const ossia::graph_node& node, Fout&& fout) noexcept
{
  for(auto port : node.root_outputs())
  {
    if(!fout(*port))
      return false;
  }
  return true;
}

}
