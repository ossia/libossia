#pragma once
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/dataflow.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/detail/small_vector.hpp>

namespace ossia
{
class graph;
struct token_request {
    ossia::time_value date{};
    double position{};
    ossia::time_value offset{};
    double speed{1.};
    bool start_discontinuous{};
    bool end_discontinuous{};

};
inline bool operator==(const token_request& lhs, const token_request& rhs) {
  return lhs.date == rhs.date && lhs.position == rhs.position
      && lhs.offset == rhs.offset && lhs.start_discontinuous == rhs.start_discontinuous
      && lhs.end_discontinuous == rhs.end_discontinuous;
}
inline bool operator!=(const token_request& lhs, const token_request& rhs) {
  return !(lhs == rhs);
}

class OSSIA_EXPORT graph_node
{
public:
  graph_node();
  virtual ~graph_node();


  bool enabled() const
  {
    return !requested_tokens.empty();
  }
  bool executed() const
  {
    return m_executed;
  }

  void set_start_discontinuous(bool b) { m_start_discontinuous = b; }
  void set_end_discontinuous(bool b) { m_end_discontinuous = b; }

  virtual bool consumes(const std::string&) const;
  virtual bool consumes(const destination&) const;
  virtual bool consumes(const execution_state&) const;

  virtual void run(token_request, execution_state&);
  virtual std::string_view label() const;

  bool has_port_inputs() const
  {
    return ossia::any_of(
          inputs(), [](const inlet_ptr& inlet) { return !inlet->sources.empty(); });
  }
  bool has_global_inputs() const
  {
    return ossia::any_of(inputs(), [&](const inlet_ptr& inlet) {
      return (inlet->scope & port::scope_t::global) && bool(inlet->address);
    });
  }
  bool has_local_inputs(const execution_state& st) const
  {
    return ossia::any_of(inputs(), [&] (const inlet_ptr& inlet) {
      if (inlet->scope & port::scope_t::local)
      {
        bool b = false;

        // TODO optimize by stopping when found
        apply_to_destination(inlet->address, st,
                             [&] (ossia::net::parameter_base* addr) {
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

  const inlets& inputs() const { return m_inlets; }
  const outlets& outputs() const { return m_outlets; }
  inlets& inputs() { return m_inlets; }
  outlets& outputs() { return m_outlets; }

  void clear();


  // These methods are only accessed by ossia::graph
  bool can_execute(const execution_state&) const
  {
    return ossia::all_of(m_inlets, [](const auto& inlet) {
      // A port can execute if : - it has source ports and all its source ports
      // have executed
      bool previous_nodes_executed
          = ossia::all_of(inlet->sources, [&](graph_edge* edge) {
          return edge->out_node->executed()
          || (!edge->out_node->enabled() /* && bool(inlet->address) */
              /* TODO check that it's in scope */);
    });

      // it does not have source ports ; we have to check for variables :
      // find if address available in local / global scope
      return !inlet->sources.empty() ? previous_nodes_executed : true // TODO
                                       ;
    });
  }

  bool start_discontinuous() const { return m_start_discontinuous; }
  bool end_discontinuous() const { return m_end_discontinuous; }

  void set_executed(bool b)
  {
    m_executed = b;
  }

  void set_prev_date(time_value d) { m_prev_date = d; }


  // incremented for each process
  int64_t temporal_counter{};
  ossia::small_vector<int64_t, 2> temporal_priority;
  ossia::small_vector<int64_t, 2> custom_priority;

  void disable()
  {
    requested_tokens.clear();
  }
  ossia::small_vector<token_request, 4> requested_tokens;
protected:
  inlets m_inlets;
  outlets m_outlets;

  ossia::time_value m_prev_date{};

  bool m_executed{};

private:

  bool m_start_discontinuous{};
  bool m_end_discontinuous{};
};
}
