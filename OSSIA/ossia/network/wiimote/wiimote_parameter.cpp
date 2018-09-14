
#include "wiimote_parameter.hpp"

#include <wiiuse.h>

using namespace ossia;
using namespace device;

namespace ossia
{
namespace net
{
//-------

rumble_parameter::rumble_parameter(
    net::node_base& node, struct wiimote_t* wiimote)
    : device_parameter(
          node, val_type::BOOL, bounding_mode::FREE, access_mode::SET,
          init_domain(val_type::BOOL))
    , m_wiimote(wiimote)
{
}

void rumble_parameter::device_update_value()
{
  const bool v = value().get<bool>();
  wiiuse_rumble(m_wiimote, v);
}

//-------

std::map<wiimote_t*, uint8_t> led_parameter::m_led_mask;

led_parameter::led_parameter(
    net::node_base& node, struct wiimote_t* wiimote, const uint8_t led)
    : device_parameter(
          node, val_type::BOOL, bounding_mode::FREE, access_mode::SET,
          init_domain(val_type::LIST))
    , m_wiimote(wiimote)
    , m_led(led)
{
  if (m_led_mask.find(wiimote) != m_led_mask.end())
    m_led_mask[m_wiimote] = 0u;
}

led_parameter::~led_parameter()
{
  m_led_mask.erase(m_wiimote);
}

void led_parameter::device_update_value()
{
  const bool v = value().get<bool>();
  const uint8_t mask = 0x10 << m_led;

  uint8_t& led_mask = m_led_mask[m_wiimote];

  if (v)
    led_mask |= mask;
  else
    led_mask &= (~mask);

  wiiuse_set_leds(m_wiimote, led_mask);
}
}
}
