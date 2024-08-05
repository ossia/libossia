#pragma once
#include <ossia/detail/hash_map.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/timer.hpp>
#include <ossia/network/context.hpp>
#include <ossia/protocols/joystick/game_controller_protocol.hpp>
#include <ossia/protocols/joystick/joystick_protocol.hpp>

#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include <ossia/detail/fmt.hpp>

namespace ossia::net
{

struct sdl_joystick_context
{
  sdl_joystick_context()
  {
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
    //  Prevent SDL from setting SIGINT handler on Posix Systems
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");

    if(int ret = SDL_Init(
           SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_SENSOR
           | SDL_INIT_GAMECONTROLLER);
       ret < 0)
      throw std::runtime_error(fmt::format("SDL Init failure: {}", SDL_GetError()));

    SDL_JoystickEventState(SDL_ENABLE);
    SDL_GameControllerEventState(SDL_ENABLE);
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

  void register_protocol(auto& protocol)
  {
    const SDL_JoystickID joystick_id = protocol.m_joystick_id;

    if(joystick_is_registered(joystick_id))
      throw std::runtime_error("A protocol is already registered for this joystick");

    {
      std::lock_guard<std::mutex> _{m_joystick_protocols_mutex};
      m_joystick_protocols[joystick_id] = &protocol;
    }
  }

  void unregister_protocol(auto& protocol)
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

  template <typename T>
  T* get_protocol_by_id(const SDL_JoystickID id)
  {
    std::lock_guard<std::mutex> _{m_joystick_protocols_mutex};
    auto it = m_joystick_protocols.find(id);
    if(it != m_joystick_protocols.end())
    {
      proto& p = it->second;
      if(auto res = ossia::get_if<T*>(&p); res && *res)
        return *res;
    }
    return nullptr;
  }

  using proto = ossia::variant<joystick_protocol*, game_controller_protocol*>;
  ossia::hash_map<SDL_JoystickID, proto> m_joystick_protocols;
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

  void push(auto* proto, ossia::net::parameter_base* param, ossia::value val)
  {
    if(proto && param)
      proto->m_device->apply_incoming_message({*proto, 0}, *param, std::move(val));
  }

  void push_axis(const SDL_JoyAxisEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id<joystick_protocol>(ev.which))
    {
      const float res = (ev.value + .5f) / (0x7FFF + .5f);
      push(p, p->m_axis_parameters[ev.axis], res);
    }
  }

  void push_axis(const SDL_ControllerAxisEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id<game_controller_protocol>(ev.which))
    {
      const float res = (ev.value + .5f) / (0x7FFF + .5f);
      push(p, p->m_axis_parameters[ev.axis], res);
    }
  }

  void push_button(const SDL_JoyButtonEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id<joystick_protocol>(ev.which))
    {
      push(p, p->m_button_parameters[ev.button], bool(ev.state == SDL_PRESSED));
    }
  }

  void push_button(const SDL_ControllerButtonEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id<game_controller_protocol>(ev.which))
    {
      push(p, p->m_button_parameters[ev.button], bool(ev.state == SDL_PRESSED));
    }
  }

  void push_sensor(const SDL_ControllerSensorEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id<game_controller_protocol>(ev.which))
    {
      push(
          p, p->m_sensor_parameters[ev.sensor],
          ossia::vec3f{ev.data[0], ev.data[1], ev.data[2]});
    }
  }

  void push_touchpad(const SDL_ControllerTouchpadEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id<game_controller_protocol>(ev.which))
    {
      auto it = p->m_touchpads.find(ev.touchpad);
      if(it == p->m_touchpads.end())
        return;

      auto& touchpad = it->second;
      if(ev.finger >= 0 && ev.finger < touchpad.fingers.size())
      {
        auto& finger = touchpad.fingers[ev.finger];
        push(p, finger.x, ev.x);
        push(p, finger.y, ev.y);
        push(p, finger.pressure, ev.pressure);
      }
    }
  }

  void push_hat(const SDL_JoyHatEvent& ev)
  {
    if(auto p = m_manager.get_protocol_by_id<joystick_protocol>(ev.which))
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

      case SDL_CONTROLLERAXISMOTION:
        push_axis(ev.caxis);
        break;
      case SDL_CONTROLLERBUTTONDOWN:
      case SDL_CONTROLLERBUTTONUP:
        push_button(ev.cbutton);
        break;
      case SDL_CONTROLLERTOUCHPADDOWN:
      case SDL_CONTROLLERTOUCHPADMOTION:
      case SDL_CONTROLLERTOUCHPADUP:
        push_touchpad(ev.ctouchpad);
        break;
      case SDL_CONTROLLERSENSORUPDATE:
        push_sensor(ev.csensor);
        break;
        // case SDL_CONTROLLERUPDATECOMPLETE_RESERVED_FOR_SDL3:
        // case SDL_CONTROLLERSTEAMHANDLEUPDATED:

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
