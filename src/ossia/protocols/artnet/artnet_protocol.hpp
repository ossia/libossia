#pragma once
#include <ossia/detail/config.hpp>
#if defined(OSSIA_PROTOCOL_ARTNET)
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/protocols/artnet/dmx_protocol_base.hpp>

using artnet_node = void*;

namespace ossia::net
{
struct dmx_config;
class OSSIA_EXPORT artnet_protocol final : public dmx_output_protocol_base
{
public:
  artnet_protocol(
      ossia::net::network_context_ptr, const dmx_config& conf, std::string_view host);
  ~artnet_protocol();

  void set_device(ossia::net::device_base& dev) override;

private:
  void update_function();

  artnet_node m_node;
};

class OSSIA_EXPORT artnet_input_protocol final : public dmx_input_protocol_base
{
public:
  artnet_input_protocol(
      ossia::net::network_context_ptr, const dmx_config& conf, std::string_view host);
  ~artnet_input_protocol();

  void set_device(ossia::net::device_base& dev) override;

private:
  void on_packet(artnet_node n, int port);
  void do_read();
  void stop() override;

  artnet_node m_node;
  std::unique_ptr<ossia::net::udp_receive_socket> m_socket;
};
}

#endif
