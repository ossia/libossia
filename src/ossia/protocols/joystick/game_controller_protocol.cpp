#include "game_controller_protocol.hpp"

#include <ossia/network/common/device_parameter.hpp>
#include <ossia/protocols/joystick/joystick_manager.hpp>

namespace ossia::net
{

game_controller_protocol::game_controller_protocol(
    ossia::net::network_context_ptr ptr, const int32_t joystick_id,
    const int joystick_index)
    : protocol_base{flags{SupportsMultiplex}}
    , m_manager{joystick_protocol_manager::instance()}
    , m_processor{joystick_event_processor::instance(m_manager)}
    , m_ctx{ptr}
    , m_joystick_id{joystick_id}
    , m_joystick_index{joystick_index}
{
  //  Check That (ID, Index) is a valid combination
  //  Could happen if a joystick is unplugged between settings and here
  if(joystick_id != SDL_JoystickGetDeviceInstanceID(joystick_index))
    throw std::runtime_error("Invalid Settings");

  //  Check that this ID is not already registered
  if(m_manager.joystick_is_registered(m_joystick_id))
    throw std::runtime_error("This Joystick is already open");

  //  Open The Joystick
  if(!SDL_IsGameController(joystick_index))
    throw std::runtime_error("This Joystick is not a game controller");

  m_joystick = SDL_GameControllerOpen(joystick_index);

  if(m_joystick == nullptr)
    throw std::runtime_error("Failed to open Joystick");

  m_processor.register_context(ptr->context);
  m_processor.start_event_loop();
}

game_controller_protocol::~game_controller_protocol()
{
  stop();
  m_processor.unregister_context(m_ctx->context);
  m_processor.stop_event_loop();
}

void game_controller_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;
  auto& root = dev.get_root_node();
  switch(SDL_GameControllerTypeForIndex(m_joystick_index))
  {
    case SDL_CONTROLLER_TYPE_XBOX360:
    case SDL_CONTROLLER_TYPE_XBOXONE:
      SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_XBOX, "1");
      break;
    case SDL_CONTROLLER_TYPE_PS4:
      SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4_RUMBLE, "1");
      break;
    case SDL_CONTROLLER_TYPE_PS5:
      SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_RUMBLE, "1");
      break;
    default:
      // TODO
      break;
  }

  // clang-format off
  static constexpr auto sensors = std::array{
      "sensor/unknown",
      "sensor/accel/main",  "sensor/gyro/main", "sensor/accel/left", "sensor/accel/right",
      "sensor/gyro/left", "sensor/gyro/right",
  };
  static constexpr auto axes = std::array{
      "stick/left/x",  "stick/left/y", "stick/right/x",
      "stick/right/y", "trigger/left", "trigger/right",
  };
  static constexpr auto buttons = std::array{
      "button/a",
      "button/b",
      "button/x",
      "button/y",
      "button/back",
      "button/guide",
      "button/start",
      "stick/left/press",
      "stick/right/press",
      "shoulder/left",
      "shoulder/right",
      "dpad/up",
      "dpad/down",
      "dpad/left",
      "dpad/right",
      "button/misc1",
      "button/paddle1",
      "button/paddle2",
      "button/paddle3",
      "button/paddle4",
      "touchpad/press"
  };
  // clang-format on

  using namespace std::literals;
  for(int i = SDL_CONTROLLER_AXIS_LEFTX; i <= SDL_CONTROLLER_AXIS_TRIGGERRIGHT; i++)
  {
    if(SDL_GameControllerHasAxis(m_joystick, static_cast<SDL_GameControllerAxis>(i))
       == SDL_TRUE)
    {
      m_axis_parameters[i] = device_parameter::create_device_parameter(
          root, axes[i], 0.0, val_type::FLOAT, bounding_mode::CLIP, access_mode::GET,
          make_domain(-1.0f, 1.0f));
    }
  }

  for(int i = SDL_CONTROLLER_BUTTON_A; i <= SDL_CONTROLLER_BUTTON_TOUCHPAD; i++)
  {
    if(SDL_GameControllerHasButton(m_joystick, static_cast<SDL_GameControllerButton>(i))
       == SDL_TRUE)
    {
      m_button_parameters[i] = device_parameter::create_device_parameter(
          root, buttons[i], false, val_type::BOOL, bounding_mode::CLIP, access_mode::GET,
          init_domain(val_type::BOOL));
    }
  }

  const int max_sensor =
#if SDL_VERSION_ATLEAST(2, 26, 0)
      SDL_SENSOR_GYRO_R
#else
      SDL_SENSOR_GYRO
#endif
      ;

  for(int i = SDL_SENSOR_UNKNOWN; i <= max_sensor; i++)
  {
    if(SDL_GameControllerHasSensor(m_joystick, static_cast<SDL_SensorType>(i))
       == SDL_TRUE)
    {
      if(SDL_GameControllerSetSensorEnabled(
             m_joystick, static_cast<SDL_SensorType>(i), SDL_TRUE)
         >= 0)
        m_sensor_parameters[i] = device_parameter::create_device_parameter(
            root, sensors[i], ossia::vec3f{}, val_type::VEC3F, bounding_mode::CLIP,
            access_mode::GET, make_domain(-1.0f, 1.0f));
    }
  }

  for(int i = 0, N = SDL_GameControllerGetNumTouchpads(m_joystick); i < N; i++)
  {
    touchpad tp;
    const int fingers = SDL_GameControllerGetNumTouchpadFingers(m_joystick, i);
    tp.fingers.resize(fingers);
    for(int f = 0; f < fingers; f++)
    {
      std::string path = "/touchpad/" + std::to_string(i) + "/" + std::to_string(f);

      tp.fingers[f].x = device_parameter::create_device_parameter(
          root, path + "/x", 0.0f, val_type::FLOAT, bounding_mode::CLIP,
          access_mode::GET, make_domain(-1.0f, 1.0f));
      tp.fingers[f].y = device_parameter::create_device_parameter(
          root, path + "/y", 0.0f, val_type::FLOAT, bounding_mode::CLIP,
          access_mode::GET, make_domain(-1.0f, 1.0f));
      tp.fingers[f].pressure = device_parameter::create_device_parameter(
          root, path + "/pressure", 0, val_type::FLOAT, bounding_mode::CLIP,
          access_mode::GET, make_domain(0.0f, 1.0f));
    }
    m_touchpads[i] = tp;
  }

  if(SDL_GameControllerHasRumble(m_joystick))
  {
    // Gamepads have two motors : a low-frequency one (left) and a high-frequency one (right)
    rumble.lo_freq = device_parameter::create_device_parameter(
        root, "/rumble/main/low_frequency", 0.0f, val_type::FLOAT, bounding_mode::CLIP,
        access_mode::SET, make_domain(0.0f, 1.0f));
    rumble.hi_freq = device_parameter::create_device_parameter(
        root, "/rumble/main/high_frequency", 0.0f, val_type::FLOAT, bounding_mode::CLIP,
        access_mode::SET, make_domain(0.0f, 1.0f));
    rumble.duration = device_parameter::create_device_parameter(
        root, "/rumble/main/duration", 0.0f, val_type::FLOAT, bounding_mode::CLIP,
        access_mode::SET, make_domain(10.0f, 1000000.0f));
  }

  if(SDL_GameControllerHasRumbleTriggers(m_joystick))
  {
    rumble_triggers.left = device_parameter::create_device_parameter(
        root, "/rumble/triggers/left", 0.0f, val_type::FLOAT, bounding_mode::CLIP,
        access_mode::SET, make_domain(0.0f, 1.0f));
    rumble_triggers.right = device_parameter::create_device_parameter(
        root, "/rumble/triggers/right", 0.0f, val_type::FLOAT, bounding_mode::CLIP,
        access_mode::SET, make_domain(0.0f, 1.0f));
    rumble_triggers.duration = device_parameter::create_device_parameter(
        root, "/rumble/triggers/duration", 0.0f, val_type::FLOAT, bounding_mode::CLIP,
        access_mode::SET, make_domain(10.0f, 1000000.0f));
  }

  m_manager.register_protocol(*this);
}

bool game_controller_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool game_controller_protocol::push(
    const net::parameter_base& param, const ossia::value& v)
{
  if(&param == rumble.duration)
  {
    uint16_t lo
        = std::clamp(ossia::convert<float>(rumble.lo_freq->value()), 0.f, 1.f) * 65535;
    uint16_t hi
        = std::clamp(ossia::convert<float>(rumble.hi_freq->value()), 0.f, 1.f) * 65535;
    uint32_t dur = ossia::convert<float>(rumble.duration->value());
    SDL_GameControllerRumble(m_joystick, lo, hi, dur);
  }

  if(&param == rumble_triggers.duration)
  {
    uint16_t left
        = std::clamp(ossia::convert<float>(rumble_triggers.left->value()), 0.f, 1.f)
          * 65535;
    uint16_t right
        = std::clamp(ossia::convert<float>(rumble_triggers.right->value()), 0.f, 1.f)
          * 65535;
    uint32_t dur = ossia::convert<float>(rumble_triggers.duration->value());
    SDL_GameControllerRumbleTriggers(m_joystick, left, right, dur);
  }
  return true;
}

bool game_controller_protocol::observe(net::parameter_base& param, bool enable)
{
  return false;
}

bool game_controller_protocol::push_raw(const ossia::net::full_parameter_data& data)
{
  return false;
}

bool game_controller_protocol::update(ossia::net::node_base&)
{
  return true;
}

void game_controller_protocol::stop()
{
  if(m_joystick != nullptr)
  {
    m_manager.unregister_protocol(*this);
    SDL_GameControllerClose(m_joystick);
    m_joystick = nullptr;
  }
}
}
