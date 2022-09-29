#pragma once
#include <ossia/detail/logger.hpp>
#include <ossia/detail/timer.hpp>
#include <ossia/network/context.hpp>
#include <ossia/protocols/joystick/joystick_protocol.hpp>

#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include <ossia/detail/fmt.hpp>

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

    if(int ret = SDL_Init(SDL_INIT_JOYSTICK); ret < 0)
      throw std::runtime_error(fmt::format("SDL Init failure: {}", SDL_GetError()));

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

  joystick_protocol_manager() { sdl_joystick_context::instance(); }

  ~joystick_protocol_manager() { SDL_Quit(); }

  bool joystick_is_registered(const SDL_JoystickID joystick_id)
  {
    return m_joystick_protocols.find(joystick_id) != m_joystick_protocols.end();
  }

  void register_protocol(joystick_protocol& protocol)
  {
    const SDL_JoystickID joystick_id = protocol.m_joystick_id;

    if(joystick_is_registered(joystick_id))
      throw std::runtime_error("A protocol is already registered for this joystick");

    {
      std::lock_guard<std::mutex> _{m_joystick_protocols_mutex};
      m_joystick_protocols[joystick_id] = &protocol;
    }
  }

  void unregister_protocol(joystick_protocol& protocol)
  {
    const SDL_JoystickID joystick_id = protocol.m_joystick_id;

    if(!joystick_is_registered(joystick_id))
      throw std::runtime_error(
          "Cannot unregister a protocol that haven't been registered");

    {
      std::lock_guard<std::mutex> _{m_joystick_protocols_mutex};
      m_joystick_protocols.erase(joystick_id);
    }
  }

  joystick_protocol* get_protocol_by_id(const SDL_JoystickID id)
  {
    joystick_protocol* ret = nullptr;

    std::lock_guard<std::mutex> _{m_joystick_protocols_mutex};
    auto it = m_joystick_protocols.find(id);
    if(it != m_joystick_protocols.end())
      ret = it->second;

    return ret;
  }

  std::map<SDL_JoystickID, joystick_protocol*> m_joystick_protocols;
  std::mutex m_joystick_protocols_mutex;
};

// TODO refactor this so that each protocol gets callback only related to its
// own joystick instead
struct joystick_event_processor
{
  struct timer_context
  {
    explicit timer_context(boost::asio::io_context& ctx)
        : context{&ctx}
        , timer{ctx}
        , count{1}
    {
    }
    timer_context(timer_context&&) = default;
    timer_context& operator=(timer_context&&) = default;

    boost::asio::io_context* context{};
    ossia::timer timer;
    int count = 0;
  };

  static inline std::atomic_int instance_count = 0;
  joystick_event_processor(joystick_protocol_manager& manager)
      : m_manager{manager}
  {
  }

  ~joystick_event_processor() = default;

  static joystick_event_processor& instance(joystick_protocol_manager& manager)
  {
    static joystick_event_processor instance{manager};
    return instance;
  }

  void register_context(boost::asio::io_context& ctx)
  {
    for(auto& tm : this->m_timers)
    {
      if(tm.context == &ctx)
      {
        tm.count++;
        return;
      }
    }

    m_timers.emplace_back(ctx);

    if(instance_count > 0)
    {
      auto& tm = m_timers.back();
      start(tm.timer);
    }
  }

  void start(ossia::timer& timer)
  {
    using namespace std::literals;
    timer.set_delay(4ms);
    timer.start([this] { this->process_events(); });
  }

  void unregister_context(boost::asio::io_context& ctx)
  {
    for(auto it = m_timers.begin(); it != m_timers.end();)
    {
      auto& tm = *it;
      if(tm.context == &ctx)
      {
        tm.count--;
        if(tm.count == 0)
        {
          it = m_timers.erase(it);
          continue;
        }
      }

      ++it;
    }
  }

  void start_event_loop()
  {
    if(instance_count++ > 0)
      return;

    for(auto& tm : m_timers)
      start(tm.timer);
  }

  void stop_event_loop()
  {
    if(--instance_count > 0)
      return;

    using namespace std::literals;
    // To be sure to quit the event loop
    SDL_Event ev;
    ev.type = SDL_FIRSTEVENT;
    SDL_PushEvent(&ev);

    for(auto& tm : m_timers)
      tm.timer.stop();
  }

  void
  push(joystick_protocol* proto, ossia::net::parameter_base* param, ossia::value val)
  {
    proto->m_device->apply_incoming_message({*proto, 0}, *param, std::move(val));
  }

  void push_axis(const SDL_JoyAxisEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id(ev.which))
    {
      const float res = (ev.value + .5f) / (0x7FFF + .5f);
      push(p, p->m_axis_parameters[ev.axis], res);
    }
  }

  void push_button(const SDL_JoyButtonEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id(ev.which))
    {
      push(p, p->m_button_parameters[ev.button], bool(ev.state == SDL_PRESSED));
    }
  }

  void push_hat(const SDL_JoyHatEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id(ev.which))
    {
      const uint8_t v = ev.value;
      float x = 0.0f, y = 0.0f;

      if(v & SDL_HAT_LEFT)
        x = -1.0;
      else if(v & SDL_HAT_RIGHT)
        x = 1.0;

      if(v & SDL_HAT_UP)
        y = 1.0;
      else if(v & SDL_HAT_DOWN)
        y = -1;

      push(p, p->m_hat_parameters[ev.hat], std::array<float, 2>{x, y});
    }
  }

  void process_event(const SDL_Event& ev)
  {
    switch(ev.type)
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
        break;
    }
  }

  void process_events()
  {
    SDL_Event ev;

    int max_event_count = 20;
    while(SDL_PollEvent(&ev) && max_event_count-- > 0)
    {
      process_event(ev);
    }
  }

  joystick_protocol_manager& m_manager;
  std::vector<timer_context> m_timers;
};

}
