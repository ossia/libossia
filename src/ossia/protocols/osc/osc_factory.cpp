#include <ossia/protocols/osc/osc_factory.hpp>
#include <ossia/protocols/osc/osc_generic_protocol.hpp>
#include <ossia/network/sockets/serial_socket.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/tcp_socket.hpp>
#include <ossia/network/sockets/unix_socket.hpp>
#include <ossia/network/sockets/websocket.hpp>
#include <ossia/network/sockets/framing.hpp>

namespace ossia::net
{
template<typename OscVersion>
std::unique_ptr<osc_protocol_base> make_osc_protocol_impl(network_context_ptr&& ctx, osc_protocol_configuration&& config)
{
  using conf = osc_protocol_configuration;

  switch(config.mode)
  {
    case conf::MIRROR:
    {
      using client_type = osc_protocol_client<OscVersion>;
      switch(config.transport)
      {
        case conf::UDP:
          return std::make_unique<osc_generic_bidir_protocol<client_type, udp_socket>>(std::move(ctx), std::get<ossia::net::double_socket_configuration>(config.configuration));

        case conf::TCP:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<client_type, tcp_size_prefix_client>>(std::move(ctx), std::get<ossia::net::socket_configuration>(config.configuration));
          else
            return std::make_unique<osc_generic_client_protocol<client_type, tcp_slip_client>>(std::move(ctx), std::get<ossia::net::socket_configuration>(config.configuration));

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
        case conf::UNIX_DGRAM:
          return std::make_unique<osc_generic_bidir_protocol<client_type, unix_datagram_socket>>(std::move(ctx), std::get<ossia::net::double_fd_configuration>(config.configuration));
        case conf::UNIX_STREAM:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<client_type, unix_stream_size_prefix_client>>(std::move(ctx), std::get<ossia::net::fd_configuration>(config.configuration));
          else
            return std::make_unique<osc_generic_client_protocol<client_type, unix_stream_slip_client>>(std::move(ctx), std::get<ossia::net::fd_configuration>(config.configuration));
#endif

        case conf::SERIAL:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<client_type, serial_socket<size_prefix_framing>>>(std::move(ctx), std::get<ossia::net::serial_configuration>(config.configuration));
          else
            return std::make_unique<osc_generic_client_protocol<client_type, serial_socket<slip_framing>>>(std::move(ctx), std::get<ossia::net::serial_configuration>(config.configuration));

        case conf::WEBSOCKETS:
          return std::make_unique<osc_generic_client_protocol<client_type, websocket_simple_client>>(std::move(ctx), std::get<ossia::net::ws_client_configuration>(config.configuration));

        default:
          break;
      }
      break;
    }
    case conf::HOST:
    {
      using client_type = osc_protocol_server<OscVersion>;
      switch(config.transport)
      {
        case conf::UDP:
          return std::make_unique<osc_generic_bidir_protocol<client_type, udp_socket>>(std::move(ctx), std::get<ossia::net::double_socket_configuration>(config.configuration));

        case conf::TCP:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_server_protocol<client_type, tcp_size_prefix_server>>(std::move(ctx), std::get<ossia::net::socket_configuration>(config.configuration));
          else
            return std::make_unique<osc_generic_server_protocol<client_type, tcp_slip_server>>(std::move(ctx), std::get<ossia::net::socket_configuration>(config.configuration));

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
        case conf::UNIX_DGRAM:
          return std::make_unique<osc_generic_bidir_protocol<client_type, unix_datagram_socket>>(std::move(ctx), std::get<ossia::net::double_fd_configuration>(config.configuration));
        case conf::UNIX_STREAM:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_server_protocol<client_type, unix_stream_size_prefix_server>>(std::move(ctx), std::get<ossia::net::fd_configuration>(config.configuration));
          else
            return std::make_unique<osc_generic_server_protocol<client_type, unix_stream_slip_server>>(std::move(ctx), std::get<ossia::net::fd_configuration>(config.configuration));

#endif

        case conf::SERIAL:
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<client_type, serial_socket<size_prefix_framing>>>(std::move(ctx), std::get<ossia::net::serial_configuration>(config.configuration));
          else
            return std::make_unique<osc_generic_client_protocol<client_type, serial_socket<slip_framing>>>(std::move(ctx), std::get<ossia::net::serial_configuration>(config.configuration));

        case conf::WEBSOCKETS:
          return std::make_unique<osc_generic_server_protocol<client_type, websocket_simple_server>>(std::move(ctx),std::get<ossia::net::ws_server_configuration>(config.configuration));

          break;
        default:
          break;
      }
      break;
    }
  }

  return {};
}

std::unique_ptr<osc_protocol_base> make_osc_protocol(network_context_ptr ctx, osc_protocol_configuration config)
{
  using conf = osc_protocol_configuration;

  switch(config.version)
  {
    case conf::OSC1_0:
      return make_osc_protocol_impl<osc_1_0_policy>(std::move(ctx), std::move(config));
    case conf::OSC1_1:
      return make_osc_protocol_impl<osc_1_1_policy>(std::move(ctx), std::move(config));
    case conf::EXTENDED:
      return make_osc_protocol_impl<osc_extended_policy>(std::move(ctx), std::move(config));
    default:
      break;
  }

  return {};
}

}
