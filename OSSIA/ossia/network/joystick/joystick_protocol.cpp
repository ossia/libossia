

#include <SDL2/SDL.h>
#include <thread>

#include "device_parameter.hpp"
#include "joystick_protocol.hpp"

namespace ossia::net
{

class joystick_protocol_manager
{

public:
  static joystick_protocol_manager& get_instance();

  bool joystick_is_registered(const SDL_JoystickID joystick_id);
  void register_protocol(joystick_protocol& protocol);
  void unregister_protocol(joystick_protocol& protoco);

  unsigned int get_joystick_count() const;
  const char* get_joystick_name(const int joystick_index) const;

  ~joystick_protocol_manager();

private:
  joystick_protocol_manager();

  void start_event_loop();
  void stop_event_loop();

  joystick_protocol* get_protocol_by_id(const SDL_JoystickID id);
  static void event_loop();

  static std::unique_ptr<joystick_protocol_manager> m_instance;
  std::map<SDL_JoystickID, joystick_protocol*> m_joystick_protocols;
  std::mutex m_joystick_protocols_mutex;

  bool m_event_loop_running{false};
  std::thread m_event_loop_thread;
};

std::unique_ptr<joystick_protocol_manager>
    joystick_protocol_manager::m_instance{};

////

joystick_protocol_manager::joystick_protocol_manager()
{
  SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
  //  Prevent SDL from setting SIGINT handler on Posix Systems
  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

  if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
    throw std::runtime_error("SDL Init failure");

  SDL_JoystickEventState(SDL_ENABLE);
}

joystick_protocol_manager::~joystick_protocol_manager()
{
  SDL_Quit();
  stop_event_loop();
}

void joystick_protocol_manager::start_event_loop()
{
  if (m_event_loop_running)
    return;
  m_event_loop_running = true;
  m_event_loop_thread = std::thread(event_loop);
}

void joystick_protocol_manager::stop_event_loop()
{
  if (!m_event_loop_running)
    return;

  m_event_loop_running = false;

  //  To be sure to quit the event loop
  SDL_Event ev;
  ev.type = SDL_FIRSTEVENT;
  SDL_PushEvent(&ev);

  if (m_event_loop_thread.joinable())
    m_event_loop_thread.join();
}

joystick_protocol_manager& joystick_protocol_manager::get_instance()
{
  if (!m_instance)
    m_instance = std::unique_ptr<joystick_protocol_manager>(
        new joystick_protocol_manager);

  return *m_instance;
}

bool joystick_protocol_manager::joystick_is_registered(
    const SDL_JoystickID joystick_id)
{
  return m_joystick_protocols.find(joystick_id) != m_joystick_protocols.end();
}

void joystick_protocol_manager::register_protocol(joystick_protocol& protocol)
{
  const SDL_JoystickID joystick_id = protocol.m_joystick_id;

  if (joystick_is_registered(joystick_id))
    throw std::runtime_error(
        "A protocol is already registered for this joystick");

  m_joystick_protocols_mutex.lock();
  m_joystick_protocols[joystick_id] = &protocol;
  m_joystick_protocols_mutex.unlock();

  //  If the manager event loop is not started, start it
  if (!m_event_loop_running)
    start_event_loop();
}

void joystick_protocol_manager::unregister_protocol(
    joystick_protocol& protocol)
{
  const SDL_JoystickID joystick_id = protocol.m_joystick_id;

  if (!joystick_is_registered(joystick_id))
    throw std::runtime_error(
        "Cannot unregister a protocol that havent been registered");

  m_joystick_protocols_mutex.lock();
  m_joystick_protocols.erase(joystick_id);
  m_joystick_protocols_mutex.unlock();

  //  If it was the last joystick, Stop The Event Loop
  if (m_joystick_protocols.size() == 0)
    stop_event_loop();
}

unsigned int joystick_protocol_manager::get_joystick_count() const
{
  SDL_JoystickUpdate();
  return static_cast<unsigned int>(SDL_NumJoysticks());
}

const char* joystick_protocol_manager::get_joystick_name(const int index) const
{
  return SDL_JoystickNameForIndex(index);
}

joystick_protocol*
joystick_protocol_manager::get_protocol_by_id(const SDL_JoystickID id)
{
  joystick_protocol* ret = nullptr;

  m_joystick_protocols_mutex.lock();
  auto it = m_joystick_protocols.find(id);

  if (it != m_joystick_protocols.end())
    ret = it->second;

  m_joystick_protocols_mutex.unlock();

  return ret;
}

void joystick_protocol_manager::event_loop()
{
  if (!m_instance)
    return;

  auto& instance = *m_instance;
  SDL_Event ev;

  while (instance.m_event_loop_running)
  {
    SDL_WaitEvent(&ev);

    switch (ev.type)
    {

      case SDL_JOYAXISMOTION:
      {
        const SDL_JoystickID joystick_id = ev.jaxis.which;

        joystick_protocol* protocol = instance.get_protocol_by_id(joystick_id);

        if (protocol != nullptr)
          protocol->m_axis_parameters[ev.jaxis.axis]->push_value(
              static_cast<float>(ev.jaxis.value / 32767.0f));
      }
      break;

      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
      {
        const SDL_JoystickID joystick_id = ev.jbutton.which;

        joystick_protocol* protocol = instance.get_protocol_by_id(joystick_id);

        if (protocol != nullptr)
          protocol->m_button_parameters[ev.jbutton.button]->push_value(
              ev.jbutton.state == SDL_PRESSED);
      }
      break;

      case SDL_JOYHATMOTION:
      {
        const SDL_JoystickID joystick_id = ev.jhat.which;

        joystick_protocol* protocol = instance.get_protocol_by_id(joystick_id);

        if (protocol != nullptr)
        {
          const uint8_t v = ev.jhat.value;
          float x = 0.0f, y = 0.0f;

          if (v & SDL_HAT_LEFT)
            x = -1.0;
          else if (v & SDL_HAT_RIGHT)
            x = 1.0;

          if (v & SDL_HAT_UP)
            y = 1.0;
          else if (v & SDL_HAT_DOWN)
            y = -1;

          protocol->m_hat_parameters[ev.jhat.hat]->push_value(
              std::array<float, 2>{x, y});
        }
      }
      break;

      default:
        std::printf(
            "Received unknown event of type %u (is not joy event)\n", ev.type);
        break;
    }
  }
}

/*
 *
 *      Joystick Protocol Implementation
 *
 */

joystick_protocol::joystick_protocol(
    const int32_t joystick_id, const int joystick_index)
    : m_joystick_id(joystick_id)
{

  auto& mgr = joystick_protocol_manager::get_instance();

  //  Check That (ID, Index) is a valid combinaison
  //  Could happen if a joystick is unpluged bettween settings and here
  if (joystick_id != SDL_JoystickGetDeviceInstanceID(joystick_index))
    throw std::runtime_error("Invalid Settings");

  //  Check that this ID is not already registered
  if (mgr.joystick_is_registered(m_joystick_id))
    throw std::runtime_error("This Joystick is already open");

  //  Open The Joystick
  m_joystick = SDL_JoystickOpen(joystick_index);

  if (m_joystick == nullptr)
    throw std::runtime_error("Failed to open Joystick");
}

joystick_protocol::~joystick_protocol()
{
  stop();
}

void joystick_protocol::set_device(ossia::net::device_base& dev)
{
  m_device = &dev;
  auto& root = dev.get_root_node();

  //  Retrive Joystick Info
  const int axis_count = SDL_JoystickNumAxes(m_joystick);
  // const int ball_count = SDL_JoystickNumBalls(m_joystick);
  const int hat_count = SDL_JoystickNumHats(m_joystick);
  const int button_count = SDL_JoystickNumButtons(m_joystick);

  //  Build PArameters Tree

  m_axis_parameters.reserve(axis_count);
  m_button_parameters.reserve(button_count);
  m_hat_parameters.reserve(hat_count);

  for (int i = 0; i < axis_count; ++i)
    m_axis_parameters.push_back(device_parameter::create_device_parameter(
        root, "axis-" + std::to_string(i + 1), 0.0, val_type::FLOAT,
        bounding_mode::CLIP, access_mode::GET, make_domain(-1.0f, 1.0f)));

  for (int i = 0; i < button_count; ++i)
    m_button_parameters.push_back(device_parameter::create_device_parameter(
        root, "button-" + std::to_string(i + 1), false, val_type::BOOL,
        bounding_mode::CLIP, access_mode::GET, init_domain(val_type::BOOL)));

  for (int i = 0; i < hat_count; ++i) {
    auto param =
        device_parameter::create_device_parameter(
            root, "hat-" + std::to_string(i + 1), std::array<float, 2>{0.0, 0.0},
            val_type::VEC2F, bounding_mode::FREE, access_mode::GET,
            init_domain(val_type::VEC2F));
    param->set_unit(ossia::cartesian_2d_u{});
    m_hat_parameters.push_back(param);
  }

  //  Register The Protocol
  auto& mgr = joystick_protocol_manager::get_instance();
  mgr.register_protocol(*this);
}

bool joystick_protocol::pull(net::parameter_base& param)
{
  return true;
}

bool joystick_protocol::push(const net::parameter_base& param)
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
  if (m_joystick != nullptr)
  {
    auto& mgr = joystick_protocol_manager::get_instance();

    mgr.unregister_protocol(*this);
    SDL_JoystickClose(m_joystick);
    m_joystick = nullptr;
  }
}

unsigned int joystick_protocol::get_joystick_count()
{
  auto& mgr = joystick_protocol_manager::get_instance();

  return mgr.get_joystick_count();
}

const char* joystick_protocol::get_joystick_name(const int index)
{
  auto& mgr = joystick_protocol_manager::get_instance();

  return mgr.get_joystick_name(index);
}

int32_t joystick_protocol::get_joystick_id(const int index)
{
  return SDL_JoystickGetDeviceInstanceID(index);
}
}
