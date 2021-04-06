#pragma once
#include <cstdint>
#if SIZE_MAX == 0xFFFFFFFF // 32-bit
#include <ossia/dataflow/audio_port.hpp>
#include <ossia/dataflow/value_port.hpp>
#include <ossia/dataflow/midi_port.hpp>
#endif

#include <ossia/dataflow/dataflow_fwd.hpp>
#include <ossia/dataflow/token_request.hpp>
#include <ossia/dataflow/exec_state_facade.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/detail/string_view.hpp>
#include <ossia/editor/scenario/time_value.hpp>


namespace ossia
{
class graph;
struct timed_value;
struct typed_value;
class state;
using token_request_vec = ossia::small_vector<token_request, 4>;

using inlets = ossia::small_vector<inlet_ptr, 2>;
using outlets = ossia::small_vector<outlet_ptr, 2>;
/*
class inlet_iterator
{
  using iterator_category = std::forward_iterator_tag;
  using value_type = ossia::inlet*;
  using difference_type = std::ptrdiff_t;
  using pointer = ossia::inlet**;
  using reference = ossia::inlet*&;
private:
  ossia::small_vector<const inlets*, 2> m_parent{};
  std::size_t m_index{};
public:
  explicit inlet_iterator(const inlets* parent, std::size_t index) noexcept
  : m_parent{parent}
  , m_index{index}
  {

  }

  inlet* operator*() const noexcept
  { return (*m_parent.back())[m_index]; }
  bool operator==(const inlet_iterator& other) const noexcept
  { return m_parent == other.m_parent && m_index == other.m_index; }
  bool operator!=(const inlet_iterator& other) const noexcept
  { return !(*this == other); }

  inlet_iterator operator++(int) noexcept
  {
      auto ret = *this;
      ++*this;
      return ret;
  }
  inlet_iterator& operator++() noexcept
  {

      return *this;
  }
};
*/

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

  void set_start_discontinuous(bool b) noexcept
  {
    m_start_discontinuous = b;
  }
  void set_end_discontinuous(bool b) noexcept
  {
    m_end_discontinuous = b;
  }

  virtual void prepare(const execution_state& st) const noexcept;
  virtual bool consumes(const execution_state&) const noexcept;
  virtual void run(const token_request&, exec_state_facade) noexcept;
  virtual std::string label() const noexcept;

  bool has_port_inputs() const noexcept;
  bool has_global_inputs() const noexcept;
  bool has_local_inputs(const execution_state& st) const noexcept;

  const inlets& root_inputs() const noexcept
  {
    return m_inlets;
  }
  const outlets& root_outputs() const noexcept
  {
    return m_outlets;
  }
  inlets& root_inputs() noexcept
  {
    return m_inlets;
  }
  outlets& root_outputs() noexcept
  {
    return m_outlets;
  }


  virtual void clear() noexcept;

  bool start_discontinuous() const noexcept
  {
    return m_start_discontinuous;
  }
  bool end_discontinuous() const noexcept
  {
    return m_end_discontinuous;
  }

  void set_executed(bool b) noexcept
  {
    m_executed = b;
  }

  void request(const ossia::token_request& req) noexcept;

  void disable() noexcept
  {
    requested_tokens.clear();
  }

  void set_logging(bool b) noexcept
  {
    m_logging = b;
  }
  bool logged() const noexcept
  {
    return m_logging;
  }

  void set_mute(bool b) noexcept
  {
    m_muted = b;
  }
  bool muted() const noexcept
  {
    return m_muted;
  }

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

class OSSIA_EXPORT nonowning_graph_node : public graph_node
{
public:
  using graph_node::graph_node;
  ~nonowning_graph_node() override;

  void clear() noexcept override;
};
}
