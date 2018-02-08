#pragma once
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/detail/small_vector.hpp>

namespace ossia
{
class graph;
struct token_request {
    token_request() = default;
    token_request(const token_request&) = default;
    token_request(token_request&&) = default;
    token_request& operator=(const token_request&) = default;
    token_request& operator=(token_request&&) = default;
    token_request(ossia::time_value d, double pos, ossia::time_value off, double s)
      : date{d}
      , position{pos}
      , offset{off}
      , speed{s}
    {
      if(offset.impl < 0)
      {
        offset.impl = 0;
      }

    }
    token_request(ossia::time_value d, double pos, ossia::time_value off)
      : token_request{d, pos, off, 1.}
    {

    }
    token_request(ossia::time_value d, double pos)
      : token_request{d, pos, time_value{}, 1.}
    {

    }
    token_request(ossia::time_value d)
      : token_request{d, 0., time_value{}, 1.}
    {

    }

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

  virtual void prepare(const execution_state& st) const;
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

  const ossia::time_value& prev_date() const
  {
    return m_prev_date;
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

  virtual void all_notes_off();
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

class OSSIA_EXPORT nonowning_graph_node: public graph_node
{
  public:
    using graph_node::graph_node;
    ~nonowning_graph_node() override;
};
}
