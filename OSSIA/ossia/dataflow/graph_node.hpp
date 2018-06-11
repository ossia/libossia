#pragma once
#include <ossia/dataflow/graph_edge.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/editor/scenario/time_value.hpp>
#include <ossia/detail/small_vector.hpp>

namespace ossia
{
class graph;
using token_request_vec = ossia::small_vector<token_request, 4>;

class OSSIA_EXPORT graph_node
{
public:
  graph_node() noexcept;
  virtual ~graph_node();

  bool enabled() const noexcept
  {
    return !requested_tokens.empty();
  }

  bool executed() const noexcept
  {
    return m_executed;
  }

  void set_start_discontinuous(bool b) noexcept { m_start_discontinuous = b; }
  void set_end_discontinuous(bool b) noexcept { m_end_discontinuous = b; }

  virtual void prepare(const execution_state& st) const noexcept;
  virtual bool consumes(const execution_state&) const noexcept;
  virtual void run(token_request, execution_state&) noexcept;
  virtual std::string label() const noexcept;

  bool has_port_inputs() const noexcept;
  bool has_global_inputs() const noexcept;
  bool has_local_inputs(const execution_state& st) const noexcept;

  const inlets& inputs() const noexcept { return m_inlets; }
  const outlets& outputs() const noexcept { return m_outlets; }
  inlets& inputs() noexcept { return m_inlets; }
  outlets& outputs() noexcept { return m_outlets; }

  virtual void clear() noexcept;

  bool start_discontinuous() const noexcept { return m_start_discontinuous; }
  bool end_discontinuous() const noexcept { return m_end_discontinuous; }

  void set_executed(bool b) noexcept
  {
    m_executed = b;
  }

  void request(ossia::token_request req) noexcept;

  void disable() noexcept
  {
    requested_tokens.clear();
  }

  void set_logging(bool b) noexcept { m_logging = b; }
  bool logged() const noexcept { return m_logging; }

  void set_mute(bool b) noexcept { m_muted = b; }
  bool muted() const noexcept { return m_muted; }

  virtual void all_notes_off() noexcept;
  token_request_vec requested_tokens;

protected:
  inlets m_inlets;
  outlets m_outlets;

  bool m_executed{};

private:
  bool m_start_discontinuous{};
  bool m_end_discontinuous{};
  bool m_logging{};
  bool m_muted{};
};

class OSSIA_EXPORT nonowning_graph_node: public graph_node
{
  public:
    using graph_node::graph_node;
    ~nonowning_graph_node() override;

    void clear() noexcept override;
};
}
