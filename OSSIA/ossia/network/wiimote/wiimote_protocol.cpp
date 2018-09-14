
#include "wiimote_protocol.hpp"
#include <wiiuse.h>

using namespace device; //  device_parameter

namespace ossia
{
namespace net
{

////

wiimote_protocol::wiimote_protocol(const bool enable_ir)
    : m_running(false), m_ready(false), m_enable_ir(enable_ir)
{
  m_wiimotes = wiiuse_init(MAX_WIIMOTES_COUNT);

  const int found
      = wiiuse_find(m_wiimotes, MAX_WIIMOTES_COUNT, 1); // 10 sec (timeout)

  if (found == 0)
  {
    wiiuse_cleanup(m_wiimotes, MAX_WIIMOTES_COUNT);
    throw std::runtime_error("No Wiimotes were found\n");
  }

  m_wiimote_count = wiiuse_connect(m_wiimotes, MAX_WIIMOTES_COUNT);

  if (m_wiimote_count == 0)
  {
    wiiuse_cleanup(m_wiimotes, MAX_WIIMOTES_COUNT);
    throw std::runtime_error("Wiimote connection failed\n");
  }
}

wiimote_protocol::~wiimote_protocol()
{
  m_running = false;
  if (m_event_thread.joinable())
    m_event_thread.join();
  wiiuse_cleanup(m_wiimotes, MAX_WIIMOTES_COUNT);
}

void wiimote_protocol::close_event_loop()
{
  m_running = false;
  if (m_event_thread.joinable())
    m_event_thread.join();
}

void wiimote_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;

  if (m_running)
    close_event_loop();

  for (unsigned int i = 0; i < m_wiimote_count; ++i)
    create_wiimote_parameters(i);

  m_ready = true;
  m_running = true;

  m_event_thread = std::thread(wiimote_event_loop, this);
}

bool wiimote_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool wiimote_protocol::push(const net::parameter_base& param)
{
  return true;
}

bool wiimote_protocol::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool wiimote_protocol::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool wiimote_protocol::update(ossia::net::node_base&)
{
  return true;
}

void wiimote_protocol::create_wiimote_parameters(const unsigned int wiimote_id)
{
  const std::string path{"wiimote-" + std::to_string(wiimote_id) + "/"};
  auto& root = m_device->get_root_node();

  wiimote_parameters& parameters = m_wiimotes_parameters[wiimote_id];

  //  Buttons parameters
  parameters.button_parameters[WIIMOTE_BUTTON_ONE]
      = device_parameter::create_device_parameter(
          root, path + "button-1", false, val_type::BOOL, bounding_mode::FREE,
          access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_TWO]
      = device_parameter::create_device_parameter(
          root, path + "button-2", false, val_type::BOOL, bounding_mode::FREE,
          access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_A]
      = device_parameter::create_device_parameter(
          root, path + "button-A", false, val_type::BOOL, bounding_mode::FREE,
          access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_B]
      = device_parameter::create_device_parameter(
          root, path + "button-B", false, val_type::BOOL, bounding_mode::FREE,
          access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_MINUS]
      = device_parameter::create_device_parameter(
          root, path + "button-minus", false, val_type::BOOL,
          bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_PLUS]
      = device_parameter::create_device_parameter(
          root, path + "button-plus", false, val_type::BOOL,
          bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_HOME]
      = device_parameter::create_device_parameter(
          root, path + "button-home", false, val_type::BOOL,
          bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_LEFT]
      = device_parameter::create_device_parameter(
          root, path + "button-left", false, val_type::BOOL,
          bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_RIGHT]
      = device_parameter::create_device_parameter(
          root, path + "button-right", false, val_type::BOOL,
          bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_DOWN]
      = device_parameter::create_device_parameter(
          root, path + "button-down", false, val_type::BOOL,
          bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));

  parameters.button_parameters[WIIMOTE_BUTTON_UP]
      = device_parameter::create_device_parameter(
          root, path + "button-up", false, val_type::BOOL, bounding_mode::FREE,
          access_mode::GET, init_domain(val_type::BOOL));

  //  Rumble parameter

  device_parameter::create_device_parameter<rumble_parameter>(
      root, path + "rumble", false, m_wiimotes[wiimote_id]);

  // leds

  for (unsigned int i = 0; i < 4; i++)
    device_parameter::create_device_parameter<led_parameter>(
        root, path + "led-" + std::to_string(i), false, m_wiimotes[wiimote_id],
        i);

  //  Motion

  wiiuse_motion_sensing(m_wiimotes[wiimote_id], 1); //  enable motion

  parameters.wiimote_axis = device_parameter::create_device_parameter(
      root, path + "wiimote-axis", std::array<float, 3>{0.0, 0.0, 0.0},
      val_type::VEC3F, bounding_mode::FREE, access_mode::GET,
      init_domain(val_type::VEC3F));
  parameters.wiimote_axis->set_unit(ossia::euler_u{});

  parameters.wiimote_gravity = device_parameter::create_device_parameter(
      root, path + "wiimote-gravity", std::array<float, 3>{0.0, 0.0, 0.0},
      val_type::VEC3F, bounding_mode::FREE, access_mode::GET,
      init_domain(val_type::VEC3F));
  parameters.wiimote_gravity->set_unit(ossia::cartesian_3d_u{});

  // Nunchuk

  // Buttons
  parameters.nunchuk_button_c = device_parameter::create_device_parameter(
      root, path + "nunchuk-button-c", false, val_type::BOOL,
      bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));

  parameters.nunchuk_button_z = device_parameter::create_device_parameter(
      root, path + "nunchuk-button-z", false, val_type::BOOL,
      bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));

  // motion
  parameters.nunchuk_axis = device_parameter::create_device_parameter(
      root, path + "nunchuk-axis", std::array<float, 3>{0.0, 0.0, 0.0},
      val_type::VEC3F, bounding_mode::FREE, access_mode::GET,
      init_domain(val_type::VEC3F));
  parameters.nunchuk_axis->set_unit(ossia::euler_u{});

  parameters.nunchuk_gravity = device_parameter::create_device_parameter(
      root, path + "nunchuk-gravity", std::array<float, 3>{0.0, 0.0, 0.0},
      val_type::VEC3F, bounding_mode::FREE, access_mode::GET,
      init_domain(val_type::VEC3F));
  parameters.nunchuk_gravity->set_unit(ossia::cartesian_3d_u{});

  parameters.nunchuk_joystick = device_parameter::create_device_parameter(
      root, path + "nunchuk-joystick", std::array<float, 2>{0.0, 0.0},
      val_type::VEC2F, bounding_mode::FREE, access_mode::GET,
      init_domain(val_type::VEC2F));
  parameters.nunchuk_joystick->set_unit(ossia::cartesian_2d_u{});

  if (m_enable_ir)
  {
    wiiuse_set_ir(m_wiimotes[wiimote_id], 1); //  enable ir

    parameters.ir_cursor = device_parameter::create_device_parameter(
        root, path + "ir-cursor", std::array<float, 2>{0.0, 0.0},
        val_type::VEC2F, bounding_mode::FREE, access_mode::GET,
        init_domain(val_type::VEC2F));

    parameters.ir_detection = device_parameter::create_device_parameter(
        root, path + "ir-detection", false, val_type::BOOL,
        bounding_mode::FREE, access_mode::GET, init_domain(val_type::BOOL));
  }
}

void wiimote_protocol::wiimote_event_loop(wiimote_protocol* self)
{
  const unsigned int wiimote_count = self->m_wiimote_count;

  while (self->m_running)
  {

    // std::this_thread::sleep_for(std::chrono::milliseconds(500));

    const int ret = wiiuse_poll(self->m_wiimotes, wiimote_count);

    if (ret > 0)
    {

      for (unsigned int i = 0; i < wiimote_count; ++i)
      {

        const WIIUSE_EVENT_TYPE ev_type = self->m_wiimotes[i]->event;

        switch (ev_type)
        {

          case WIIUSE_EVENT:
            handle_wiimote_event(self, i);
            break;

          default:
            break;
        }
      }
    }
  }
}

void wiimote_protocol::handle_wiimote_event(
    wiimote_protocol* self, const unsigned int wiimote_id)
{
  wiimote_parameters& parameters = self->m_wiimotes_parameters[wiimote_id];

  wiimote_t* wiimote = self->m_wiimotes[wiimote_id];

  //-

  for (auto& cpl : parameters.button_parameters)
    cpl.second->push_value(
        IS_PRESSED(self->m_wiimotes[wiimote_id], cpl.first));

  // Update wiimote axis

  parameters.wiimote_axis->push_value(std::array<float, 3>{
      wiimote->orient.yaw, wiimote->orient.pitch, wiimote->orient.roll});

  parameters.wiimote_gravity->push_value(std::array<float, 3>{
      wiimote->gforce.x, wiimote->gforce.y, wiimote->gforce.z});

  if (self->m_enable_ir)
  {
    //  Update Ir cursor

    unsigned int n = 0u;

    for (unsigned int i = 0; i < 4; ++i)
      if (wiimote->ir.dot[i].visible)
        n++;

    //  If at least one ir dot is visible
    if (n >= 1u)
    {
      parameters.ir_cursor->push_value(std::array<float, 2>{
          (static_cast<float>(wiimote->ir.x + wiimote->ir.offset[0]))
              / static_cast<float>(wiimote->ir.vres[0]),
          (static_cast<float>(wiimote->ir.y + wiimote->ir.offset[1]))
              / static_cast<float>(wiimote->ir.vres[1])});
      parameters.ir_detection->push_value(true);
    }
    else
    {
      parameters.ir_detection->push_value(false);
    }
  }

  // If a nunchuk is pluged
  if (self->m_wiimotes[wiimote_id]->exp.type == EXP_NUNCHUK)
  {
    struct nunchuk_t* nunchuk
        = (nunchuk_t*)&(self->m_wiimotes[wiimote_id]->exp.nunchuk);

    // Update nunchuk parameters

    parameters.nunchuk_axis->push_value(std::array<float, 3>{
        nunchuk->orient.yaw, nunchuk->orient.pitch, nunchuk->orient.roll});

    parameters.nunchuk_gravity->push_value(std::array<float, 3>{
        nunchuk->gforce.x, nunchuk->gforce.y, nunchuk->gforce.z});

    parameters.nunchuk_joystick->push_value(
        std::array<float, 2>{nunchuk->js.x, nunchuk->js.y});

    parameters.nunchuk_button_c->push_value(
        IS_PRESSED(nunchuk, NUNCHUK_BUTTON_C));

    parameters.nunchuk_button_z->push_value(
        IS_PRESSED(nunchuk, NUNCHUK_BUTTON_Z));
  }
}
}
}
