#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/nodes/spline/spline2d.hpp>
#include <ossia/dataflow/nodes/spline/spline3d.hpp>
#include <ossia/dataflow/port.hpp>

// Courtesy of tinyspline library, MIT license.
#include <ossia/editor/automation/tinyspline_util.hpp>
#include <ossia/editor/curve/behavior.hpp>

namespace ossia::nodes
{

class spline final : public ossia::nonowning_graph_node
{
public:
  spline() { m_outlets.push_back(&value_out); }

  ~spline() override = default;

  std::string label() const noexcept override { return "spline"; }

  void set_spline(const spline_data& t)
  {
    m_spline.set_points(
        reinterpret_cast<const tsReal*>(t.points.data()), t.points.size());
  }

private:
  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_spline)
      return;

    ossia::value_port& vp = *value_out;
    const double pos = ossia::clamp(t.position(), 0., 1.);

    const auto [res_x, res_y] = m_spline.evaluate(pos);

    const auto [tick_start, d] = e.timings(t);

    vp.write_value(
        ossia::make_vec(m_x + m_scaleX * res_x, m_y + m_scaleY * res_y), tick_start);
  }

  ossia::value_outlet value_out;
  ts::spline<2> m_spline;
  double m_x{}, m_y{};
  static constexpr double m_scaleX{1.}, m_scaleY{1.};
};

class spline3d final : public ossia::nonowning_graph_node
{
public:
  spline3d() { m_outlets.push_back(&value_out); }

  ~spline3d() override = default;

  std::string label() const noexcept override { return "spline"; }

  void set_spline(const spline3d_data& t)
  {
    m_spline.set_points(
        reinterpret_cast<const tsReal*>(t.points.data()), t.points.size());
  }

private:
  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_spline)
      return;

    ossia::value_port& vp = *value_out;
    const double pos = ossia::clamp(t.position(), 0., 1.);

    const auto [res_x, res_y, res_z] = m_spline.evaluate(pos);

    const auto [tick_start, d] = e.timings(t);

    vp.write_value(
        ossia::make_vec(
            m_x + m_scaleX * res_x, m_y + m_scaleY * res_y, m_z + m_scaleZ * res_z),
        tick_start);
  }

  ossia::value_outlet value_out;
  ts::spline<3> m_spline;
  double m_x{}, m_y{}, m_z{};
  static constexpr double m_scaleX{1.}, m_scaleY{1.}, m_scaleZ{1.};
};
}
