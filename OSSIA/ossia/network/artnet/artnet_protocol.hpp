
#pragma once

#include <array>
#include <cstdint>
#include <thread>


#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>

#define DMX_CHANNEL_COUNT 512

typedef void *artnet_node;

namespace ossia
{
namespace net
{

class OSSIA_EXPORT artnet_protocol final : public ossia::net::protocol_base
{
  public:

  struct dmx_buffer {
    dmx_buffer();
    int send(artnet_node node);
    
    uint8_t data[DMX_CHANNEL_COUNT];
    bool dirty;
  };

  artnet_protocol(const unsigned int update_frequency);
  ~artnet_protocol();

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base& param) override;
  bool push(const ossia::net::parameter_base& param) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base& param, bool enable) override;

  bool update(ossia::net::node_base&) override;

private:
  static void update_function(artnet_protocol *instance);

  std::thread m_update_thread;
  bool m_running;
  const unsigned int m_update_frequency;
  dmx_buffer m_buffer;

  ossia::net::device_base* m_device{};
  artnet_node m_node;
};
}
}
