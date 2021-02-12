#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/context.hpp>
#include <asio/high_resolution_timer.hpp>
#include <array>
#include <cstdint>
#include <thread>

#define DMX_CHANNEL_COUNT 512

typedef void* artnet_node;

namespace ossia
{
namespace net
{

class OSSIA_EXPORT artnet_protocol final : public ossia::net::protocol_base
{
public:
  struct dmx_buffer
  {
    dmx_buffer();
    int send(artnet_node node);

    uint8_t data[DMX_CHANNEL_COUNT];
    bool dirty;
  };

  artnet_protocol(ossia::net::network_context_ptr, const unsigned int update_frequency);
  ~artnet_protocol();

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base& param) override;
  bool push(const ossia::net::parameter_base& param, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base& param, bool enable) override;

  bool update(ossia::net::node_base&) override;

private:
  void update_function();

  ossia::net::network_context_ptr m_context;

  using clock = std::chrono::high_resolution_clock;
  asio::high_resolution_timer m_timer;
  clock::time_point m_prev_time{};
  std::chrono::milliseconds m_delay{};
  dmx_buffer m_buffer;

  ossia::net::device_base* m_device{};
  artnet_node m_node;
};
}
}
#endif
