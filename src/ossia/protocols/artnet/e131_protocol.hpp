
#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/protocols/artnet/dmx_buffer.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/context.hpp>
#include <ossia/detail/timer.hpp>
#include <array>
#include <cstdint>

namespace ossia::net
{
// Implementation mostly based on https://github.com/hhromic/libe131

class OSSIA_EXPORT e131_protocol final
    : public ossia::net::protocol_base
{
public:
  static constexpr uint16_t default_port = 5568;
  static constexpr uint8_t default_priority = 100;

  e131_protocol(ossia::net::network_context_ptr, const dmx_config& conf,
                const ossia::net::socket_configuration& socket
                );

  ~e131_protocol();

  void set_device(ossia::net::device_base& dev) override;

  bool pull(ossia::net::parameter_base& param) override;
  bool push(const ossia::net::parameter_base& param, const ossia::value& v) override;
  bool push_raw(const ossia::net::full_parameter_data&) override;
  bool observe(ossia::net::parameter_base& param, bool enable) override;

  bool update(ossia::net::node_base&) override;

  dmx_buffer& buffer() noexcept { return m_buffer; }
private:
  void update_function();

  ossia::net::network_context_ptr m_context;

  ossia::timer m_timer;
  dmx_buffer m_buffer;

  ossia::net::device_base* m_device{};

  ossia::net::udp_send_socket m_socket;
  uint16_t m_universe{};
  bool m_autocreate{};
};
}
#endif
