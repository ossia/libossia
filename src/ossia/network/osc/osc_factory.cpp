#include <ossia/network/osc/osc_factory.hpp>

#include <ossia/detail/mutex.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/lockfree_queue.hpp>

#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/osc/detail/osc_protocol_common.hpp>
#include <ossia/network/osc/detail/osc_1_0_policy.hpp>
#include <ossia/network/osc/detail/osc_1_1_policy.hpp>
#include <ossia/network/osc/detail/osc_1_1_extended_policy.hpp>

#include <tsl/hopscotch_map.h>

#include <atomic>
#include <string>

namespace ossia::net
{
template<typename OscMode, typename Socket>
class osc_generic_protocol
    : public ossia::net::protocol_base
{
public:
  using socket_type = Socket;

  // Constructor for TCP / UDP
  osc_generic_protocol(
      network_context_ptr ctx,
      std::string_view local_host,  uint16_t local_port,
      std::string_view remote_host, uint16_t remote_port)
    : protocol_base{flags{}}
    , m_ctx{std::move(ctx)}
    , m_id{*this}
    , from_client{local_host, local_port, m_ctx->context}
    , to_client{remote_host, remote_port, m_ctx->context}
  {
    OscMode::init(*this);
  }

  // Constructor for UNIX / Serial
  osc_generic_protocol(
      network_context_ptr ctx,
      std::string_view local_fd, std::string_view remote_fd)
    : protocol_base{flags{}}
    , m_ctx{std::move(ctx)}
    , m_id{*this}
    , from_client{local_fd, m_ctx->context}
    , to_client{remote_fd, m_ctx->context}
  {
    OscMode::init(*this);
  }

  osc_generic_protocol(
      network_context_ptr ctx, const fd_configuration& conf)
    : osc_generic_protocol{ctx, conf.read_fd, conf.write_fd}
  {
  }

  osc_generic_protocol(
      network_context_ptr ctx, const socket_configuration& conf)
    : osc_generic_protocol{
        ctx,
        conf.local_host, conf.local_port,
        conf.remote_host, conf.remote_port}
  {
  }

  osc_generic_protocol(const osc_generic_protocol&) = delete;
  osc_generic_protocol(osc_generic_protocol&&) = delete;
  osc_generic_protocol& operator=(const osc_generic_protocol&) = delete;
  osc_generic_protocol& operator=(osc_generic_protocol&&) = delete;

  ~osc_generic_protocol() override
  {
  }

  bool learning() const
  {
    return m_learning;
  }

  osc_generic_protocol& set_learning(bool v)
  {
    m_learning = v;
    return *this;
  }

  bool update(ossia::net::node_base& node_base) override
  {
    return false;
  }

  bool pull(ossia::net::parameter_base& parameter_base) override
  {
    return false;
  }

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override
  {
    return OscMode::observe(*this, parameter_base, enable);
  }

  bool echo_incoming_message(
      const message_origin_identifier& id, const parameter_base& addr, const value& val) override
  {
    return OscMode::echo_incoming_message(*this, id, addr, val);
  }

  bool push(const ossia::net::parameter_base& addr, const ossia::value& v) override
  {
    return OscMode::push(*this, addr, v);
  }

  bool push(const ossia::net::parameter_base& addr, ossia::value&& v) override
  {
    return OscMode::push(*this, addr, std::move(v));
  }

  bool push_raw(const ossia::net::full_parameter_data& addr) override
  {
    return OscMode::push_raw(*this, addr);
  }

  bool push_bundle(
      const std::vector<const parameter_base*>& addresses) override
  {
    return OscMode::push_bundle(*this, to_client, addresses);
  }

  bool push_raw_bundle(
      const std::vector<ossia::net::full_parameter_data>& addresses) override
  {
    return OscMode::push_raw_bundle(*this, to_client, addresses);
  }

  void on_received_message(const oscpack::ReceivedMessage& m)
  {
    return OscMode::on_received_message(*this, m);
  }

  void set_device(ossia::net::device_base& dev) override
  {
    m_device = &dev;
  }

  using ossia::net::protocol_base::m_logger;
  ossia::net::network_context_ptr m_ctx;
  message_origin_identifier m_id;
  listened_parameters m_listening;

  ossia::net::device_base* m_device{};

  Socket from_client;
  Socket to_client;

  std::atomic_bool m_learning{};
};
template<typename OscVersion>
std::unique_ptr<ossia::net::protocol_base> make_osc_protocol_impl(network_context_ptr&& ctx, osc_protocol_configuration&& config)
{
  using conf = osc_protocol_configuration;

  switch(config.mode)
  {
    case conf::CLIENT:
    {
      using client_type = osc_protocol_client<OscVersion>;
      switch(config.transport)
      {
        case conf::UDP:
          return std::make_unique<osc_generic_protocol<client_type, udp_socket>>(std::move(ctx), std::get<socket_configuration>(config.configuration));
        case conf::TCP:
          return std::make_unique<osc_generic_protocol<client_type, tcp_socket>>(std::move(ctx), std::get<socket_configuration>(config.configuration));
        case conf::UNIX:
          return std::make_unique<osc_generic_protocol<client_type, unix_socket>>(std::move(ctx), std::get<fd_configuration>(config.configuration));
        case conf::SERIAL:
          return {}; // TODO
        case conf::WEBSOCKETS:
          return {}; // TODO
          break;
        default:
          break;
      }
      break;
    }
    case conf::SERVER:
    {
      using client_type = osc_protocol_server<OscVersion>;
      switch(config.transport)
      {
        case conf::UDP:
          return std::make_unique<osc_generic_protocol<client_type, udp_socket>>(std::move(ctx), std::get<socket_configuration>(config.configuration));
        case conf::TCP:
          return std::make_unique<osc_generic_protocol<client_type, tcp_socket>>(std::move(ctx), std::get<socket_configuration>(config.configuration));
        case conf::UNIX:
          return std::make_unique<osc_generic_protocol<client_type, unix_socket>>(std::move(ctx), std::get<fd_configuration>(config.configuration));
        case conf::SERIAL:
          return {}; // TODO
        case conf::WEBSOCKETS:
          return {}; // TODO
          break;
        default:
          break;
      }
      break;
    }
  }

  return{};
}

std::unique_ptr<ossia::net::protocol_base> make_osc_protocol(network_context_ptr ctx, osc_protocol_configuration config)
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

  return{};
}

}
