#pragma once
#include <ossia/dataflow/connection.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/detail/apply.hpp>

namespace ossia
{
struct init_delay_line
{
  delay_line_type& delay_line;
  void operator ()(const audio_port&) { delay_line = audio_delay_line{}; }
  void operator ()(const value_port&) { delay_line = value_delay_line{}; }
  void operator ()(const midi_port&) { delay_line = midi_delay_line{}; }
  void operator ()() { }
};

// A pure dependency edge does not have in/out ports set
struct OSSIA_EXPORT graph_edge
{
  graph_edge(
      connection c, outlet_ptr pout, inlet_ptr pin, node_ptr pout_node,
      node_ptr pin_node);

  graph_edge(
      connection c, std::size_t pout, std::size_t pin, node_ptr pout_node, node_ptr pin_node);

  void init();

  ~graph_edge();

  void clear();

  connection con;
  outlet_ptr out;
  inlet_ptr in;
  node_ptr out_node;
  node_ptr in_node;
};

template <typename... Args>
auto make_edge(Args&&... args)
{
  return std::make_shared<ossia::graph_edge>(std::forward<Args>(args)...);
}

template <typename... Args>
auto make_strict_edge(Args&&... args)
{
  return std::make_shared<ossia::graph_edge>(ossia::immediate_strict_connection{}, std::forward<Args>(args)...);
}
template <typename... Args>
auto make_glutton_edge(Args&&... args)
{
  return std::make_shared<ossia::graph_edge>(ossia::immediate_glutton_connection{}, std::forward<Args>(args)...);
}

template <typename... Args>
auto make_delayed_strict_edge(Args&&... args)
{
  return std::make_shared<ossia::graph_edge>(ossia::delayed_strict_connection{}, std::forward<Args>(args)...);
}
template <typename... Args>
auto make_delayed_glutton_edge(Args&&... args)
{
  return std::make_shared<ossia::graph_edge>(ossia::delayed_glutton_connection{}, std::forward<Args>(args)...);
}


}
