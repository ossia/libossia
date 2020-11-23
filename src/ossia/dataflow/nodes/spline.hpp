#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

// Courtesy of tinyspline library, MIT license.
#include <ossia/editor/automation/tinyspline_util.hpp>
#include <ossia/editor/curve/behavior.hpp>

namespace ossia::nodes
{
struct spline_point
{
  double x;
  double y;

  friend bool operator==(const spline_point& lhs, const spline_point& rhs)
  {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
  friend bool operator!=(const spline_point& lhs, const spline_point& rhs)
  {
    return lhs.x != rhs.x || lhs.y != rhs.y;
  }
};

struct spline_data
{
  std::vector<spline_point> points;

  friend bool operator==(const spline_data& lhs, const spline_data& rhs)
  {
    return lhs.points == rhs.points;
  }
  friend bool operator!=(const spline_data& lhs, const spline_data& rhs)
  {
    return lhs.points != rhs.points;
  }
};

struct spline3d_point
{
  double x;
  double y;
  double z;

  friend bool operator==(const spline3d_point& lhs, const spline3d_point& rhs)
  {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
  }
  friend bool operator!=(const spline3d_point& lhs, const spline3d_point& rhs)
  {
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
  }
};

struct spline3d_data
{
  std::vector<spline3d_point> points;

  friend bool operator==(const spline3d_data& lhs, const spline3d_data& rhs)
  {
    return lhs.points == rhs.points;
  }
  friend bool operator!=(const spline3d_data& lhs, const spline3d_data& rhs)
  {
    return lhs.points != rhs.points;
  }
};

class spline final : public ossia::nonowning_graph_node
{
public:
  spline()
  {
    m_outlets.push_back(&value_out);
  }

  ~spline() override
  {
  }

  std::string label() const noexcept override
  {
    return "spline";
  }


  void set_spline(const spline_data& t)
  {
    m_spline.set_points(reinterpret_cast<const tsReal*>(t.points.data()), t.points.size());
  }

private:
  void run(const ossia::token_request& t, ossia::exec_state_facade e) noexcept override
  {
    if(!m_spline)
      return;

    ossia::value_port& vp = *value_out;
    const double pos = [&] { double p = t.position(); return p >= 0. ? p : 0.; }();

    auto [res_x, res_y] = m_spline.evaluate(pos);

    const auto tick_start = e.physical_start(t);

    vp.write_value(
        ossia::make_vec(
            m_x + m_scaleX * res_x, m_y + m_scaleY * res_y),
        tick_start);
  }

  ossia::value_outlet value_out;
  ts::spline<2> m_spline;
  double m_x{}, m_y{}, m_scaleX{1.}, m_scaleY{1.};
};
}
