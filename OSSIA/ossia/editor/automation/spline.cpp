// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/editor/automation/spline.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/state/state_element.hpp>

namespace ossia
{

spline_automation::spline_automation()
{
}

void spline_automation::set_destination(Destination a)
{
  m_address = std::move(a);
}

void spline_automation::set_spline(const spline_data& t)
{
  m_spline = tinyspline::BSpline(3, 2, t.points.size(), TS_CLAMPED);

  ts_bspline_set_ctrlp(
      m_spline.data(),
      reinterpret_cast<const tinyspline::real*>(t.points.data()),
      m_spline.data());
}

ossia::state_element
spline_automation::state(ossia::time_value date, double pos)
{
  if (m_address)
  {
    auto& addr = *m_address;

    auto p = m_spline.evaluate(pos);
    auto d = p.data();
    return ossia::message{addr,
                          ossia::vec2f{float(m_x + m_scaleX * d->result[0]),
                                       float(m_y + m_scaleY * d->result[1])}};
  }
  return {};
}

void spline_automation::start(ossia::state& st)
{
}

void spline_automation::stop()
{
}

void spline_automation::pause()
{
}

void spline_automation::resume()
{
}
}
