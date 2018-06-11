#pragma once
#include <ossia/dataflow/graph_node.hpp>
#include <ossia/dataflow/port.hpp>

// Courtesy of tinyspline library, MIT license.
#include <ossia/editor/automation/tinysplinecpp.h>

#include <ossia/editor/curve/behavior.hpp>

namespace ossia::nodes
{
struct spline_point
{
  double x() const
  {
    return m_x;
  }
  double y() const
  {
    return m_y;
  }

  double m_x;
  double m_y;

  friend bool operator==(const spline_point& lhs, const spline_point& rhs)
  {
    return lhs.m_x == rhs.m_x && lhs.m_y == rhs.m_y;
  }
  friend bool operator!=(const spline_point& lhs, const spline_point& rhs)
  {
    return lhs.m_x != rhs.m_x || lhs.m_y != rhs.m_y;
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

class spline final :
    public ossia::nonowning_graph_node
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
      m_spline = tinyspline::BSpline(3, 2, t.points.size(), TS_CLAMPED);

      ts_bspline_set_ctrlp(
          m_spline.data(),
          reinterpret_cast<const tinyspline::real*>(t.points.data()),
          m_spline.data());
    }

  private:
    void run(ossia::token_request t, ossia::exec_state_facade e) noexcept override
    {
      ossia::value_port* vp = value_out.data.target<ossia::value_port>();

      auto p = m_spline.evaluate(t.position);
      auto d = p.data();

      vp->add_value(
            ossia::make_vec(
                      m_x + m_scaleX * d->result[0],
                      m_y + m_scaleY * d->result[1]), t.date);
    }

    ossia::outlet value_out{ossia::value_port{}};
    tinyspline::BSpline m_spline;
    double m_x{}, m_y{}, m_scaleX{1.}, m_scaleY{1.};
};
}
