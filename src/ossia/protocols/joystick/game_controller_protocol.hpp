#pragma once

#include <ossia/detail/small_flat_map.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/common/device_parameter.hpp>
#include <ossia/network/context_functions.hpp>
#include <ossia/network/domain/domain.hpp>

typedef struct _SDL_GameController SDL_GameController;
namespace ossia::net
{
class joystick_protocol_manager;
struct joystick_event_processor;

class OSSIA_EXPORT game_controller_protocol final : public ossia::net::protocol_base
{
  friend class joystick_protocol_manager;
  friend struct joystick_event_processor;

public:
  game_controller_protocol(
      ossia::net::network_context_ptr, const int32_t joystick_id,
      const int joystick_index);
  game_controller_protocol(const game_controller_protocol&) = delete;
  game_controller_protocol(game_controller_protocol&&) = delete;
  game_controller_protocol& operator=(const game_controller_protocol&) = delete;
  game_controller_protocol& operator=(game_controller_protocol&&) = delete;
  ~game_controller_protocol() override;

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base& param) override;
  bool push(const ossia::net::parameter_base& param, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base& param, bool enable) override;

  bool update(ossia::net::node_base&) override;
  void stop() override;

private:
  joystick_protocol_manager& m_manager;
  joystick_event_processor& m_processor;
  ossia::net::network_context_ptr m_ctx{};

  ossia::net::device_base* m_device{};

  ossia::small_flat_map<int, device_parameter*, 6> m_axis_parameters;
  ossia::small_flat_map<int, device_parameter*, 32> m_button_parameters;
  ossia::small_flat_map<int, device_parameter*, 10> m_sensor_parameters;

  struct
  {
    device_parameter* lo_freq{};
    device_parameter* hi_freq{};
    device_parameter* duration{};
  } rumble;

  struct
  {
    device_parameter* left{};
    device_parameter* right{};
    device_parameter* duration{};
  } rumble_triggers;
  struct touchpad
  {
    struct finger
    {
      device_parameter* x{};
      device_parameter* y{};
      device_parameter* pressure{};
    };
    std::vector<finger> fingers;
  };
  ossia::small_flat_map<int, touchpad, 2> m_touchpads;

  int32_t m_joystick_id{};
  int32_t m_joystick_index{};

  SDL_GameController* m_joystick{};
};

}
