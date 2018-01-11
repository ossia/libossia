#pragma once
#include <ossia/dataflow/dataflow_fwd.hpp>
namespace ossia
{
class graph_interface
{
public:
  virtual ~graph_interface();
  virtual void add_node(ossia::node_ptr) = 0;
  virtual void remove_node(const ossia::node_ptr&) = 0;

  virtual void connect(ossia::edge_ptr) = 0;
  virtual void disconnect(const ossia::edge_ptr&) = 0;
  virtual void disconnect(ossia::graph_edge*) = 0;

  virtual void mark_dirty() = 0;

  virtual void state(execution_state& e) = 0;

  virtual void clear() = 0;

  virtual void print(std::ostream&) = 0;
};
}
