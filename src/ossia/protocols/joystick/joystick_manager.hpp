#pragma once
#include <ossia/network/context.hpp>
#include <ossia/protocols/joystick/joystick_protocol.hpp>

#include <ossia/detail/logger.hpp>
#include <ossia/detail/timer.hpp>

#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include <thread>

namespace ossia::net
{

struct sdl_joystick_context
{
  sdl_joystick_context()
  {
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    //  Prevent SDL from setting SIGINT handler on Posix Systems
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if (SDL_Init(SDL_INIT_JOYSTICK) < 0)
      throw std::runtime_error("SDL Init failure");

    SDL_JoystickEventState(SDL_ENABLE);
  }

  static sdl_joystick_context& instance()
  {
    static sdl_joystick_context instance{};
    return instance;
  }

  ~sdl_joystick_context()
  {
    //  To be sure to quit the event loop
    SDL_Event ev;
    ev.type = SDL_FIRSTEVENT;
    SDL_PushEvent(&ev);

    SDL_Quit();
  }
};

class joystick_protocol_manager
{
public:
  static joystick_protocol_manager& instance()
  {
    static joystick_protocol_manager instance{};
    return instance;
  }

  joystick_protocol_manager()
  {
    sdl_joystick_context::instance();
  }

  ~joystick_protocol_manager()
  {
    SDL_Quit();
  }

  bool joystick_is_registered(
      const SDL_JoystickID joystick_id)
  {
    return m_joystick_protocols.find(joystick_id) != m_joystick_protocols.end();
  }

  void register_protocol(joystick_protocol& protocol)
  {
    const SDL_JoystickID joystick_id = protocol.m_joystick_id;

    if (joystick_is_registered(joystick_id))
      throw std::runtime_error(
          "A protocol is already registered for this joystick");

    { std::lock_guard<std::mutex> _{m_joystick_protocols_mutex};
      m_joystick_protocols[joystick_id] = &protocol;
    }
  }

  void unregister_protocol(
      joystick_protocol& protocol)
  {
    const SDL_JoystickID joystick_id = protocol.m_joystick_id;

    if (!joystick_is_registered(joystick_id))
      throw std::runtime_error(
          "Cannot unregister a protocol that havent been registered");

    {
      std::lock_guard<std::mutex> _{m_joystick_protocols_mutex};
      m_joystick_protocols.erase(joystick_id);
    }
  }

  joystick_protocol*
  get_protocol_by_id(const SDL_JoystickID id)
  {
    joystick_protocol* ret = nullptr;

    std::lock_guard<std::mutex> _{m_joystick_protocols_mutex};
    auto it = m_joystick_protocols.find(id);
    if (it != m_joystick_protocols.end())
      ret = it->second;

    return ret;
  }

  std::map<SDL_JoystickID, joystick_protocol*> m_joystick_protocols;
  std::mutex m_joystick_protocols_mutex;
};

struct joystick_event_processor
{
  static inline int instance_count = 0;
  joystick_event_processor(joystick_protocol_manager& manager, asio::io_context& ctx)
    : m_manager{manager}
    , m_timer{ctx}
  {
    int cur = instance_count++;
    if(cur == 0)
      start_event_loop();
  }

  ~joystick_event_processor()
  {
    instance_count--;
    if(instance_count == 0)
      stop_event_loop();
  }

  static joystick_event_processor& instance(joystick_protocol_manager& manager, asio::io_context& ctx)
  {
    static joystick_event_processor instance{manager, ctx};
    return instance;
  }

  void start_event_loop()
  {
    if (m_event_loop_running)
      return;
    m_event_loop_running = true;

    using namespace std::literals;
    m_timer.set_delay(4ms);
    m_timer.start([this] { this->process_events(); });
  }

  void stop_event_loop()
  {
    if (!m_event_loop_running)
      return;

    m_event_loop_running = false;

    // To be sure to quit the event loop
    SDL_Event ev;
    ev.type = SDL_FIRSTEVENT;
    SDL_PushEvent(&ev);

    m_timer.stop();
  }

  void push(joystick_protocol* proto, ossia::net::parameter_base* param, ossia::value val)
  {
    proto->m_device->apply_incoming_message({*proto, 0}, *param, std::move(val));
  }

  void push_axis(const SDL_JoyAxisEvent& ev)
  {
    if (auto p = m_manager.get_protocol_by_id(ev.which))
    {
      push(p, p->m_axis_parameters[ev.axis], float(ev.value / 32767.0f));
    }
  }

  void push_button(const SDL_JoyButtonEvent& ev)
  {
    if (auto p = m_manager.get_protocol_by_id(ev.which))
    {
      push(p, p->m_button_parameters[ev.button], bool(ev.state == SDL_PRESSED));
    }
  }

  void push_hat(const SDL_JoyHatEvent& ev)
  {
    if (auto p = m_manager.get_protocol_by_id(ev.which))
    {
      const uint8_t v = ev.value;
      float x = 0.0f, y = 0.0f;

      if (v & SDL_HAT_LEFT)
        x = -1.0;
      else if (v & SDL_HAT_RIGHT)
        x = 1.0;

      if (v & SDL_HAT_UP)
        y = 1.0;
      else if (v & SDL_HAT_DOWN)
        y = -1;

      push(p, p->m_hat_parameters[ev.hat], std::array<float, 2>{x, y});
    }
  }

  void process_event(const SDL_Event& ev)
  {
    switch (ev.type)
    {
      case SDL_JOYAXISMOTION:
        push_axis(ev.jaxis);
        break;

      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
        push_button(ev.jbutton);
        break;

      case SDL_JOYHATMOTION:
        push_hat(ev.jhat);
        break;

      default:
        ossia::logger().info("Received unknown event of type {} (is not joy event)", ev.type);
        break;
    }
  }

  void process_events()
  {
    SDL_Event ev;

    int max_event_count = 20;
    while (SDL_PollEvent(&ev) && max_event_count-- > 0)
    {
      process_event(ev);
    }
  }

  joystick_protocol_manager& m_manager;
  ossia::timer m_timer;

  bool m_event_loop_running{false};
};

////

}
