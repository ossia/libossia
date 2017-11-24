#pragma once
#include <ossia/dataflow/execution_state.hpp>
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <chobo/small_vector.hpp>

namespace ossia
{
class graph;
struct token_request {
    ossia::time_value date{};
    double position{};
    ossia::time_value offset{};
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

  bool enabled() const;
  bool executed() const;

  void set_start_discontinuous(bool b) { m_start_discontinuous = b; }
  void set_end_discontinuous(bool b) { m_end_discontinuous = b; }

  virtual bool consumes(const std::string&) const;
  virtual bool consumes(const destination&) const;
  virtual bool consumes(const execution_state&) const;

  virtual void run(token_request, execution_state&);

  bool has_port_inputs() const;
  bool has_global_inputs() const;
  bool has_local_inputs(const execution_state&) const;

  const inlets& inputs() const;
  const outlets& outputs() const;
  inlets& inputs();
  outlets& outputs();

  void clear();


  // These methods are only accessed by ossia::graph
  bool can_execute(const execution_state&) const;

  bool start_discontinuous() const { return m_start_discontinuous; }
  bool end_discontinuous() const { return m_end_discontinuous; }

  void set_executed(bool b);

  void set_prev_date(time_value d) { m_prev_date = d; }


  // incremented for each process
  int64_t temporal_counter{};
  chobo::small_vector<int64_t, 2> temporal_priority;
  chobo::small_vector<int64_t, 2> custom_priority;

  void disable();
  chobo::small_vector<token_request, 4> requested_tokens;
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
