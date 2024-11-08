
#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/protocols/artnet/dmx_protocol_base.hpp>

namespace ossia::net
{
// Implementation mostly based on https://github.com/hhromic/libe131

class OSSIA_EXPORT e131_protocol final : public ossia::net::dmx_output_protocol_base
{
public:
  static constexpr uint16_t default_port = 5568;
  static constexpr uint8_t default_priority = 100;

  e131_protocol(
      ossia::net::network_context_ptr, const dmx_config& conf,
      const ossia::net::outbound_socket_configuration& socket);

  ~e131_protocol();

  void set_device(ossia::net::device_base& dev) override;

private:
  void update_function();
  ossia::net::udp_send_socket m_socket;
};

class OSSIA_EXPORT e131_input_protocol final : public ossia::net::dmx_input_protocol_base
{
public:
  static constexpr uint16_t default_port = 5568;
  static constexpr uint8_t default_priority = 100;

  e131_input_protocol(
      ossia::net::network_context_ptr, const dmx_config& conf,
      const ossia::net::inbound_socket_configuration& socket);

  ~e131_input_protocol();

  void set_device(ossia::net::device_base& dev) override;

private:
  void on_packet(const char* bytes, int sz);

  ossia::net::udp_receive_socket m_socket;
};
}
#endif
