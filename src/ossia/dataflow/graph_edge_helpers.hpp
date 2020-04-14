#pragma once
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/graph_node.hpp>

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

}
