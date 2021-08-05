#pragma once
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/graph/graph_interface.hpp>
#include <ossia/dataflow/port.hpp>

namespace ossia
{

inline auto make_strict_edge(int pout, int pin, ossia::node_ptr nout, ossia::node_ptr nin)
{
  return make_edge(
      ossia::immediate_strict_connection{},
      nout->root_outputs()[pout],
      nin->root_inputs()[pin],
      nout, nin
      );
}

inline auto make_glutton_edge(int pout, int pin, ossia::node_ptr nout, ossia::node_ptr nin)
{
  return make_edge(
        ossia::immediate_glutton_connection{},
        nout->root_outputs()[pout],
        nin->root_inputs()[pin],
        nout, nin
        );
}

inline auto make_delayed_strict_edge(int pout, int pin, ossia::node_ptr nout, ossia::node_ptr nin)
{
  return make_edge(
        ossia::delayed_strict_connection{},
        nout->root_outputs()[pout],
        nin->root_inputs()[pin],
        nout, nin
        );
}

inline auto make_delayed_glutton_edge(int pout, int pin, ossia::node_ptr nout, ossia::node_ptr nin)
{
  return make_edge(
        ossia::delayed_glutton_connection{},
        nout->root_outputs()[pout],
        nin->root_inputs()[pin],
        nout, nin
        );
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

  void operator()()
  {
    auto& old_inputs = node->root_inputs();
    auto& old_outputs = node->root_outputs();
    {
      int k = 0;
      for(auto& old_in : old_inputs)
      {
        if(k < inls.size())
        {
          auto& new_in = inls[k];
          for(std::size_t child_i = 0; child_i < old_in->child_inlets.size(); child_i++)
          {
            auto cld = old_in->child_inlets[child_i];
            for(auto& child_cable : cld->cables())
            {
              child_cable->in = new_in->child_inlets[child_i];
            }
          }

          for(auto& cable : old_in->cables())
          {
            cable->in = new_in;
          }
          new_in->cables() = std::move(old_in->cables());
        }
        else
        {
          for(ossia::graph_edge* cable : old_in->cables())
          {
            graph->disconnect(cable);
            cable->clear();
          }
          old_in->cables().clear();
        }

        delete old_in;
        k++;
      }
      old_inputs.clear();
    }

    {
      int k = 0;
      for(auto& old_out : old_outputs)
      {
        if(k < outls.size())
        {
          auto& new_out = outls[k];
          for(std::size_t child_i = 0; child_i < old_out->child_inlets.size(); child_i++)
          {
            auto cld = old_out->child_inlets[child_i];
            for(auto& child_cable : cld->cables())
            {
              child_cable->in = new_out->child_inlets[child_i];
            }
          }

          for(auto& cable : old_out->cables())
          {
            cable->out = new_out;
          }
          new_out->cables() = std::move(old_out->cables());
        }
        else
        {
          for(ossia::graph_edge* cable : old_out->cables())
          {
            graph->disconnect(cable);
            cable->clear();
          }
          old_out->cables().clear();
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
