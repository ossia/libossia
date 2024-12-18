#include <ossia/network/sockets/framing.hpp>
#include <ossia/network/sockets/null_socket.hpp>
#include <ossia/network/sockets/serial_socket.hpp>
#include <ossia/network/sockets/tcp_socket.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/unix_socket.hpp>
#include <ossia/network/sockets/websocket.hpp>
#include <ossia/protocols/osc/osc_factory.hpp>
#include <ossia/protocols/osc/osc_generic_protocol.hpp>

namespace ossia::net
{
template <typename OscVersion>
std::unique_ptr<osc_protocol_base>
make_osc_protocol_impl(network_context_ptr&& ctx, osc_protocol_configuration&& config)
{
  using conf = osc_protocol_configuration;

  switch(config.mode)
  {
    case conf::MIRROR: {
      using client_type = osc_protocol_client<OscVersion>;
      struct
      {
        network_context_ptr&& ctx;
        osc_protocol_configuration& config;
        auto operator()(ossia::net::udp_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          if(conf.remote && conf.local)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, udp_send_socket, udp_receive_socket>>(
                std::move(ctx), *conf.remote, *conf.local);
          else if(conf.remote)
            return std::make_unique<
                osc_generic_bidir_protocol<client_type, udp_send_socket, null_socket>>(
                std::move(ctx), *conf.remote);
          else if(conf.local)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, null_socket, udp_receive_socket>>(
                std::move(ctx), *conf.local);
          else
            return {};
        }

        auto operator()(ossia::net::tcp_client_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<
                osc_generic_client_protocol<client_type, tcp_size_prefix_client>>(
                std::move(ctx), conf);
          else
            return std::make_unique<
                osc_generic_client_protocol<client_type, tcp_slip_client>>(
                std::move(ctx), conf);
        }

        auto operator()(ossia::net::tcp_server_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          return {};
        }

        auto operator()(ossia::net::unix_dgram_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
          if(conf.remote && conf.local)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, unix_datagram_socket, unix_datagram_socket>>(
                std::move(ctx), *conf.remote, *conf.local);
          else if(conf.remote)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, unix_datagram_socket, null_socket>>(
                std::move(ctx), *conf.remote);
          else if(conf.local)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, null_socket, unix_datagram_socket>>(
                std::move(ctx), *conf.local);
          else
            return {};
#endif
          return {};
        }

        auto operator()(ossia::net::unix_stream_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<
                client_type, unix_stream_size_prefix_client>>(std::move(ctx), conf);
          else
            return std::make_unique<
                osc_generic_client_protocol<client_type, unix_stream_slip_client>>(
                std::move(ctx), conf);
#endif
          return {};
        }

        auto operator()(ossia::net::serial_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<
                client_type, serial_socket<size_prefix_framing>>>(std::move(ctx), conf);
          else
            return std::make_unique<
                osc_generic_client_protocol<client_type, serial_socket<slip_framing>>>(
                std::move(ctx), conf);
        }

        auto operator()(ossia::net::ws_client_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          return std::make_unique<
              osc_generic_client_protocol<client_type, websocket_simple_client>>(
              std::move(ctx), conf);
        }

        auto operator()(ossia::net::ws_server_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          return {};
        }

      } vis{std::move(ctx), config};
      using namespace std;
      return visit(vis, std::move(config.transport));
    }
    case conf::HOST: {
      using client_type = osc_protocol_server<OscVersion>;
      struct
      {
        network_context_ptr&& ctx;
        osc_protocol_configuration& config;
        auto operator()(ossia::net::udp_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          if(conf.remote && conf.local)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, udp_send_socket, udp_receive_socket>>(
                std::move(ctx), *conf.remote, *conf.local);
          else if(conf.remote)
            return std::make_unique<
                osc_generic_bidir_protocol<client_type, udp_send_socket, null_socket>>(
                std::move(ctx), *conf.remote);
          else if(conf.local)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, null_socket, udp_receive_socket>>(
                std::move(ctx), *conf.local);
          else
            return {};
        }

        auto operator()(ossia::net::tcp_client_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          return {};
        }

        auto operator()(ossia::net::tcp_server_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<
                osc_generic_server_protocol<client_type, tcp_size_prefix_server>>(
                std::move(ctx), conf);
          else
            return std::make_unique<
                osc_generic_server_protocol<client_type, tcp_slip_server>>(
                std::move(ctx), conf);
        }

        auto operator()(ossia::net::unix_dgram_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
          if(conf.remote && conf.local)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, unix_datagram_socket, unix_datagram_socket>>(
                std::move(ctx), *conf.remote, *conf.local);
          else if(conf.remote)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, unix_datagram_socket, null_socket>>(
                std::move(ctx), *conf.remote);
          else if(conf.local)
            return std::make_unique<osc_generic_bidir_protocol<
                client_type, null_socket, unix_datagram_socket>>(
                std::move(ctx), *conf.local);
          else
            return {};
#endif
          return {};
        }

        auto operator()(ossia::net::unix_stream_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_server_protocol<
                client_type, unix_stream_size_prefix_server>>(std::move(ctx), conf);
          else
            return std::make_unique<
                osc_generic_server_protocol<client_type, unix_stream_slip_server>>(
                std::move(ctx), conf);
#endif
          return {};
        }

        auto operator()(ossia::net::serial_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          if(config.framing == conf::SIZE_PREFIX)
            return std::make_unique<osc_generic_client_protocol<
                client_type, serial_socket<size_prefix_framing>>>(std::move(ctx), conf);
          else
            return std::make_unique<
                osc_generic_client_protocol<client_type, serial_socket<slip_framing>>>(
                std::move(ctx), conf);
        }

        auto operator()(ossia::net::ws_client_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          return {};
        }

        auto operator()(ossia::net::ws_server_configuration&& conf) const
            -> std::unique_ptr<osc_protocol_base>
        {
          return std::make_unique<
              osc_generic_server_protocol<client_type, websocket_simple_server>>(
              std::move(ctx), conf);
        }
      } vis{std::move(ctx), config};

      using namespace std;
      return visit(vis, std::move(config.transport));
    }
  }

  return {};
}

std::unique_ptr<osc_protocol_base>
make_osc_protocol(network_context_ptr ctx, osc_protocol_configuration config)
{
  using conf = osc_protocol_configuration;

  switch(config.bundle_strategy)
  {
    case conf::NEVER_BUNDLE:
      switch(config.version)
      {
        case conf::OSC1_0:
          return make_osc_protocol_impl<osc_1_0_policy>(
              std::move(ctx), std::move(config));
        case conf::OSC1_1:
          return make_osc_protocol_impl<osc_1_1_policy>(
              std::move(ctx), std::move(config));
        case conf::EXTENDED:
          return make_osc_protocol_impl<osc_extended_policy>(
              std::move(ctx), std::move(config));
        default:
          break;
      }
      break;
    case conf::ALWAYS_BUNDLE: {
      switch(config.version)
      {
        case conf::OSC1_0:
          return make_osc_protocol_impl<osc_always_bundled_policy<osc_1_0_policy>>(
              std::move(ctx), std::move(config));
        case conf::OSC1_1:
          return make_osc_protocol_impl<osc_always_bundled_policy<osc_1_1_policy>>(
              std::move(ctx), std::move(config));
        case conf::EXTENDED:
          return make_osc_protocol_impl<osc_always_bundled_policy<osc_extended_policy>>(
              std::move(ctx), std::move(config));
        default:
          break;
      }
      break;
    }
  }

  return {};
}
}
