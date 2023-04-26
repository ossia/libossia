#pragma once
#include <ossia/dataflow/connection.hpp>
#include <ossia/dataflow/typed_value.hpp>

namespace ossia
{
struct init_delay_line
{
  delay_line_type& delay_line;
  void operator()(const audio_port&) const noexcept { delay_line = audio_delay_line{}; }
  void operator()(const value_port&) const noexcept { delay_line = value_delay_line{}; }
  void operator()(const midi_port&) const noexcept { delay_line = midi_delay_line{}; }
  void operator()(const geometry_port&) const noexcept
  {
    delay_line = geometry_delay_line{};
  }
  void operator()() const noexcept { }
};

class graph_interface;
/*
template <typename... Args>
std::shared_ptr<ossia::graph_edge> make_edge(Args&&... args);
*/
// A pure dependency edge does not have in/out ports set
struct OSSIA_EXPORT graph_edge
{
  friend graph_interface;
  /*
  template <typename... Args>
  friend std::shared_ptr<ossia::graph_edge> make_edge(Args&&...);
*/
private:
  graph_edge() = default;

public:
  graph_edge(
      connection c, outlet_ptr pout, inlet_ptr pin, node_ptr pout_node,
      node_ptr pin_node) noexcept;
  ~graph_edge();

  void init() noexcept;
  void clear() noexcept;

  static std::size_t size_of_allocated_memory_by_make_shared() noexcept;

  connection con{};
  outlet_ptr out{};
  inlet_ptr in{};
  node_ptr out_node{};
  node_ptr in_node{};
};
/*
template <typename... Args>
std::shared_ptr<ossia::graph_edge> make_edge(Args&&... args)
{
  return std::shared_ptr<ossia::graph_edge>(
      new ossia::graph_edge(std::forward<Args>(args)...));
}
*/
}
