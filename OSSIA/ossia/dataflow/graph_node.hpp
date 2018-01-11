#pragma once
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/port.hpp>
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
      && lhs.offset == rhs.offset && lhs.speed == rhs.speed && lhs.start_discontinuous == rhs.start_discontinuous
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

  virtual bool consumes(const execution_state&) const;
  virtual void run(token_request, execution_state&);
  virtual std::string label() const;

  bool has_port_inputs() const;
  bool has_global_inputs() const;
  bool has_local_inputs(const execution_state& st) const;

  const inlets& inputs() const { return m_inlets; }
  const outlets& outputs() const { return m_outlets; }
  inlets& inputs() { return m_inlets; }
  outlets& outputs() { return m_outlets; }

  void clear();

  bool start_discontinuous() const { return m_start_discontinuous; }
  bool end_discontinuous() const { return m_end_discontinuous; }

  void set_executed(bool b)
  {
    m_executed = b;
  }

  void set_prev_date(time_value d)
  {
    m_prev_date = d;
  }

  void disable()
  {
    requested_tokens.clear();
  }

  void set_logging(bool b) { m_logging = b; }
  bool logged() const { return m_logging; }

  ossia::small_vector<token_request, 4> requested_tokens;

protected:
  inlets m_inlets;
  outlets m_outlets;

  ossia::time_value m_prev_date{};

  bool m_executed{};

private:
  bool m_start_discontinuous{};
  bool m_end_discontinuous{};
  bool m_logging{};
};
}
