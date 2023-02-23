#pragma once
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia
{

inline auto make_strict_edge(
    graph_interface& g, int pout, int pin, ossia::node_ptr nout, ossia::node_ptr nin)
{
  return g.allocate_edge(
      ossia::immediate_strict_connection{}, nout->root_outputs()[pout],
      nin->root_inputs()[pin], nout, nin);
}

inline auto make_glutton_edge(
    graph_interface& g, int pout, int pin, ossia::node_ptr nout, ossia::node_ptr nin)
{
  return g.allocate_edge(
      ossia::immediate_glutton_connection{}, nout->root_outputs()[pout],
      nin->root_inputs()[pin], nout, nin);
}

inline auto make_delayed_strict_edge(
    graph_interface& g, int pout, int pin, ossia::node_ptr nout, ossia::node_ptr nin)
{
  return g.allocate_edge(
      ossia::delayed_strict_connection{}, nout->root_outputs()[pout],
      nin->root_inputs()[pin], nout, nin);
}

inline auto make_delayed_glutton_edge(
    graph_interface& g, int pout, int pin, ossia::node_ptr nout, ossia::node_ptr nin)
{
  return g.allocate_edge(
      ossia::delayed_glutton_connection{}, nout->root_outputs()[pout],
      nin->root_inputs()[pin], nout, nin);
}

/**
 * Replaces the ports of a node by a new set of ports
 */
struct recabler
{
  std::shared_ptr<ossia::graph_node> node;
  std::shared_ptr<ossia::graph_interface> graph;
  ossia::inlets inls;
  ossia::outlets outls;

  void clear_cables(ossia::inlet& port)
  {
    // FIXME this allocates
    auto cbl = port.cables();
    for(ossia::graph_edge* cable : cbl)
    {
      graph->disconnect(cable);
      cable->clear();
    }
    port.cables().clear();
  }
  void clear_cables(ossia::outlet& port)
  {
    auto cbl = port.cables();
    for(ossia::graph_edge* cable : cbl)
    {
      graph->disconnect(cable);
      cable->clear();
    }
    port.cables().clear();
  }

  template <typename T>
  void copy_child_inlets(T* old_port, T* new_port)
  {
    for(std::size_t child_i = 0; child_i < old_port->child_inlets.size(); child_i++)
    {
      auto old_cld = old_port->child_inlets[child_i];
      if(child_i < new_port->child_inlets.size())
      {
        auto new_cld = new_port->child_inlets[child_i];

        copy_port(old_cld, new_cld);
      }
      else
      {
        clear_cables(*old_cld);
      }
    }
  }

  void copy_port(ossia::inlet* old_in, ossia::inlet* new_in)
  {
    copy_child_inlets(old_in, new_in);

    if(old_in->which() == new_in->which())
    {
      new_in->address = old_in->address;
      for(auto& cable : old_in->cables())
      {
        cable->in = new_in;
      }
      new_in->cables() = std::move(old_in->cables());
    }
    else
    {
      clear_cables(*old_in);
    }
  }

  void copy_port(ossia::outlet* old_out, ossia::outlet* new_out)
  {
    copy_child_inlets(old_out, new_out);

    if(old_out->which() == new_out->which())
    {
      new_out->address = old_out->address;
      for(auto& cable : old_out->cables())
      {
        cable->out = new_out;
      }
      new_out->cables() = std::move(old_out->cables());
    }
    else
    {
      clear_cables(*old_out);
    }
  }

  void operator()()
  {
    auto& old_inputs = node->root_inputs();
    auto& old_outputs = node->root_outputs();
    {
      std::size_t k = 0;
      for(auto& old_in : old_inputs)
      {
        if(k < inls.size())
        {
          auto& new_in = inls[k];
          copy_port(old_in, new_in);
        }
        else
        {
          clear_cables(*old_in);
        }

        delete old_in;
        k++;
      }
      old_inputs.clear();
    }

    {
      std::size_t k = 0;
      for(auto& old_out : old_outputs)
      {
        if(k < outls.size())
        {
          auto& new_out = outls[k];
          copy_port(old_out, new_out);
        }
        else
        {
          clear_cables(*old_out);
        }

        delete old_out;
        k++;
      }

      old_outputs.clear();
    }

    using namespace std;
    swap(node->root_inputs(), inls);
    swap(node->root_outputs(), outls);
  }
};

}
