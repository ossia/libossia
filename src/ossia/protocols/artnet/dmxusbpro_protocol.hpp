#pragma once

#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/network/sockets/serial_socket.hpp>
#include <ossia/protocols/artnet/dmx_protocol_base.hpp>

#include <boost/asio/serial_port.hpp>

namespace ossia::net
{
class OSSIA_EXPORT dmxusbpro_protocol final : public dmx_protocol_base
{
public:
  dmxusbpro_protocol(
      ossia::net::network_context_ptr, const dmx_config& conf,
      const ossia::net::serial_configuration& socket, int version);

  ~dmxusbpro_protocol();

  void set_device(ossia::net::device_base& dev) override;

private:
  void update_function(uint8_t command);

  boost::asio::serial_port m_port;
  int m_version = 1;
};
}
#endif
