#include "joystick_protocol.hpp"

#include <ossia/network/common/device_parameter.hpp>
#include <ossia/protocols/joystick/joystick_manager.hpp>

namespace ossia::net
{

joystick_protocol::joystick_protocol(
    ossia::net::network_context_ptr ptr, const int32_t joystick_id,
    const int joystick_index)
    : protocol_base{flags{SupportsMultiplex}}
    , m_manager{joystick_protocol_manager::instance()}
    , m_processor{joystick_event_processor::instance(m_manager)}
    , m_ctx{ptr}
    , m_joystick_id{joystick_id}
{
  //  Check That (ID, Index) is a valid combination
  //  Could happen if a joystick is unplugged between settings and here
  if(joystick_id != SDL_JoystickGetDeviceInstanceID(joystick_index))
    throw std::runtime_error("Invalid Settings");

  //  Check that this ID is not already registered
  if(m_manager.joystick_is_registered(m_joystick_id))
    throw std::runtime_error("This Joystick is already open");

  //  Open The Joystick
  m_joystick = SDL_JoystickOpen(joystick_index);

  if(m_joystick == nullptr)
    throw std::runtime_error("Failed to open Joystick");

  m_processor.register_context(ptr->context);
  m_processor.start_event_loop();
}

joystick_protocol::~joystick_protocol()
{
  stop();
  m_processor.unregister_context(m_ctx->context);
  m_processor.stop_event_loop();
}

void joystick_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;
  auto& root = dev.get_root_node();

  //  Retrieve Joystick Info
  const int axis_count = SDL_JoystickNumAxes(m_joystick);
  // const int ball_count = SDL_JoystickNumBalls(m_joystick);
  const int hat_count = SDL_JoystickNumHats(m_joystick);
  const int button_count = SDL_JoystickNumButtons(m_joystick);

  //  Build Parameters Tree

  m_axis_parameters.reserve(axis_count);
  m_button_parameters.reserve(button_count);
  m_hat_parameters.reserve(hat_count);

  for(int i = 0; i < axis_count; ++i)
    m_axis_parameters.push_back(device_parameter::create_device_parameter(
        root, "axis-" + std::to_string(i + 1), 0.0, val_type::FLOAT, bounding_mode::CLIP,
        access_mode::GET, make_domain(-1.0f, 1.0f)));

  for(int i = 0; i < button_count; ++i)
    m_button_parameters.push_back(device_parameter::create_device_parameter(
        root, "button-" + std::to_string(i + 1), false, val_type::BOOL,
        bounding_mode::CLIP, access_mode::GET, init_domain(val_type::BOOL)));

  for(int i = 0; i < hat_count; ++i)
  {
    auto param = device_parameter::create_device_parameter(
        root, "hat-" + std::to_string(i + 1), std::array<float, 2>{0.0, 0.0},
        val_type::VEC2F, bounding_mode::FREE, access_mode::GET,
        init_domain(val_type::VEC2F));
    param->set_unit(ossia::cartesian_2d_u{});
    m_hat_parameters.push_back(param);
  }

  //  Register The Protocol
  m_manager.register_protocol(*this);
}

bool joystick_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool joystick_protocol::push(const net::parameter_base& param, const ossia::value& v)
{
  return true;
}

bool joystick_protocol::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool joystick_protocol::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool joystick_protocol::update(ossia::net::node_base&)
{
  return true;
}

void joystick_protocol::stop()
{
  if(m_joystick != nullptr)
  {
    m_manager.unregister_protocol(*this);
    SDL_JoystickClose(m_joystick);
    m_joystick = nullptr;
  }
}

unsigned int joystick_info::get_joystick_count()
{
  sdl_joystick_context::instance();
  SDL_JoystickUpdate();
  return static_cast<unsigned int>(SDL_NumJoysticks());
}

const char* joystick_info::get_joystick_name(const int index)
{
  sdl_joystick_context::instance();
  return SDL_JoystickNameForIndex(index);
}

bool joystick_info::get_joystick_is_gamepad(const int index)
{
  sdl_joystick_context::instance();
  return SDL_IsGameController(index);
}

int32_t joystick_info::get_joystick_id(const int index)
{
  sdl_joystick_context::instance();
  return SDL_JoystickGetDeviceInstanceID(index);
}

void joystick_info::write_joystick_uuid(const int index, uint8_t* dst)
{
  sdl_joystick_context::instance();
  const auto uid = SDL_JoystickGetDeviceGUID(index);
  std::copy_n(&uid.data[0], 16, dst);
}

std::pair<int32_t, int32_t>
joystick_info::get_available_id_for_uid(const uint8_t* request)
{
  auto& mgr = joystick_protocol_manager::instance();
  SDL_JoystickUpdate();
  for(int i = 0, N = SDL_NumJoysticks(); i < N; i++)
  {
    const auto uid = SDL_JoystickGetDeviceGUID(i);
    if(std::equal(std::begin(uid.data), std::end(uid.data), request))
    {
      auto id = SDL_JoystickGetDeviceInstanceID(i);
      if(mgr.joystick_is_registered(id))
        continue;

      return {id, i};
    }
  }
  return {-1, -1};
}
}
