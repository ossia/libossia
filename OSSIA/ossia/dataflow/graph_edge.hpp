#pragma once
#include <ossia/dataflow/connection.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/apply.hpp>

namespace ossia
{
// A pure dependency edge does not have in/out ports set
struct graph_edge
{
  graph_edge(connection c, outlet_ptr pout, inlet_ptr pin,
             node_ptr pout_node, node_ptr pin_node):
    con{c},
    out{std::move(pout)},
    in{std::move(pin)},
    out_node{std::move(pout_node)},
    in_node{std::move(pin_node)}
  {
    if(in && out)
    {
      out->connect(this);
      in->connect(this);

      if(auto delay = con.target<delayed_glutton_connection>())
      {
        delay->buffer = out->data;
        ossia::apply(clear_data{}, delay->buffer);
      }
      else if(auto sdelay = con.target<delayed_strict_connection>())
      {
        sdelay->buffer = out->data;
        ossia::apply(clear_data{}, sdelay->buffer);
      }
    }
  }

  ~graph_edge()
  {
    clear();
  }

  void clear()
  {
    if(in && out)
    {
      out->disconnect(this);
      in->disconnect(this);
    }

    con = connection{};
    out.reset();
    in.reset();
    out_node.reset();
    in_node.reset();
  }

  connection con;
  outlet_ptr out;
  inlet_ptr in;
  node_ptr out_node;
  node_ptr in_node;
};

template<typename... Args>
auto make_edge(Args&&... args)
{
  return std::make_shared<ossia::graph_edge>(std::forward<Args>(args)...);
}

}
