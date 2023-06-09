#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/dataflow/control_inlets.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/node_process.hpp>
#include <ossia/dataflow/port.hpp>
#include <ossia/editor/automation/curve_value_visitor.hpp>
#include <ossia/editor/curve/behavior.hpp>

/**
 * \file automation.hpp
 */

namespace ossia::nodes
{
/**
 * \brief The ossia::nodes::automation class
 *
 * An automation is a time process that represents a curve of one
 * or multiple parameters.
 *
 * It drives (i.e. sends messages) to a parameter, according to a driving
 * value. Its driven \ref net::parameter_base can be either of numeric type
 * (\ref val_type::INT, \ref val_type::FLOAT...) or of \ref val_type::LIST, if
 * the list only has numeric elements (e.g. List{Float, Float}).
 *
 * The driving \ref value can either be a single \ref Behavior or a \ref List
 * of \ref Behan ,e to the type of the driven \ref net::parameter_base.
 *
 * The automation has a "source" domain, i.e. the data space in which the
 * transformation
 * happens. The target domain is taken from the driven parameter_base.
 * The unit is stored in m_lastMessage.unit.
 *
 *
 * \see \ref behavior \ref curve \ref curve_segment
 */
class automation final : public ossia::nonowning_graph_node
{
public:
  automation() { m_outlets.push_back(&value_out); }

  ~automation() override = default;

  [[nodiscard]] std::string label() const noexcept override { return "automation"; }

  void set_behavior(const ossia::behavior& b) { m_drive = b; }

  void reset_drive() { m_drive.reset(); }

private:
  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_drive)
      return;
    const auto [tick_start, d] = e.timings(t);

    ossia::value_port& vp = *value_out;
    vp.write_value(
        ossia::apply(
            ossia::detail::compute_value_visitor{t.position(), ossia::val_type::FLOAT},
            m_drive),
        tick_start);
  }

  ossia::behavior m_drive;
  ossia::value_outlet value_out;
};

class float_automation final : public ossia::nonowning_graph_node
{
public:
  float_automation() { m_outlets.push_back(&value_out); }

  ~float_automation() override = default;

  std::string label() const noexcept override { return "automation (float)"; }

  void set_behavior(ossia::curve<double, float> b) { m_drive = std::move(b); }

  void reset_drive() { m_drive.reset(); }

private:
  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    const auto [tick_start, d] = e.timings(t);

    ossia::value_port& vp = *value_out;
    vp.write_value(m_drive.value_at(t.position()), tick_start);
  }

  ossia::curve<double, float> m_drive;
  ossia::minmax_float_outlet value_out;
};

#if defined(OSSIA_SCENARIO_DATAFLOW)
class automation_process final : public ossia::node_process
{
public:
  using ossia::node_process::node_process;
  void start() override
  {
    static_cast<ossia::nodes::automation*>(node.get())->reset_drive();
  }
};
#endif
}
