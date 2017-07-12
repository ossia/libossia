#include <ossia/editor/automation/gradient.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <ossia/editor/scenario/time_constraint.hpp>
#include <ossia/editor/curve/curve_segment/easing.hpp>

namespace ossia
{
color_automation::color_automation()
{

}

void color_automation::set_destination(Destination a)
{
  a.unit = ossia::argb_u{};
  m_address = std::move(a);
}

void color_automation::set_gradient(color_automation::grad_type t)
{
  m_data = std::move(t);
}

state_element color_automation::state(ossia::time_value, double pos)
{
  if(m_address)
  {
    auto& addr = *m_address;

    switch(m_data.size())
    {
    case 0:
      return ossia::message{addr, ossia::vec4f{0., 0., 0., 0.}};
    case 1:
      return ossia::message{addr, ossia::argb{m_data.begin()->second}.dataspace_value};
    }

    auto it_next = m_data.lower_bound(pos);
    // Before start
    if(it_next == m_data.begin())
      return ossia::message{addr, ossia::argb{m_data.begin()->second}.dataspace_value};

    // past end
    if(it_next == m_data.end())
      return ossia::message{addr, ossia::argb{m_data.rbegin()->second}.dataspace_value};

    auto it_prev = it_next;
    --it_prev;

    // Interpolate in hsv domain
    const ossia::hunter_lab prev{it_prev->second};
    const ossia::hunter_lab next{it_next->second};

    const auto coeff = (pos - it_prev->first) / (it_next->first - it_prev->first);

    ossia::hunter_lab res;
    ossia::easing::ease e{};
    res.dataspace_value = ossia::make_vec(
          e(prev.dataspace_value[0], next.dataspace_value[0], coeff),
        e(prev.dataspace_value[1], next.dataspace_value[1], coeff),
        e(prev.dataspace_value[2], next.dataspace_value[2], coeff)
        );

    return ossia::message{addr, ossia::argb{res}.dataspace_value};
  }
  return {};
}

void color_automation::start()
{
}

void color_automation::stop()
{
}

void color_automation::pause()
{
}

void color_automation::resume()
{
}

}
