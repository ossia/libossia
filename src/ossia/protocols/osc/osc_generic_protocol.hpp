#pragma once
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/mutex.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/osc/detail/osc_1_0_policy.hpp>
#include <ossia/network/osc/detail/osc_1_1_extended_policy.hpp>
#include <ossia/network/osc/detail/osc_1_1_policy.hpp>
#include <ossia/network/osc/detail/osc_protocol_common.hpp>
#include <ossia/network/sockets/configuration.hpp>
#include <ossia/network/sockets/null_socket.hpp>
#include <ossia/network/sockets/writers.hpp>
#include <ossia/network/value/value.hpp>

#include <ossia/detail/hash_map.hpp>

#include <atomic>
#include <string>

namespace ossia::net
{
template <typename OscMode, typename SendSocket, typename RecvSocket>
class osc_generic_bidir_protocol final : public can_learn<ossia::net::protocol_base>
{
public:
  using osc_configuration = typename OscMode::osc_configuration;
  static constexpr bool bundled = requires { typename osc_configuration::bundled; };
  using writer_type = socket_writer<SendSocket>;

  osc_generic_bidir_protocol(
      network_context_ptr ctx, const send_fd_configuration& send_conf,
      const receive_fd_configuration& recv_conf)
      : can_learn<ossia::net::protocol_base>{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , from_client{recv_conf, m_ctx->context}
      , to_client{send_conf, m_ctx->context}
  {
    from_client.open();
    to_client.connect();

    from_client.receive([this](const char* data, std::size_t sz) {
      if(!m_device)
        return;
      auto on_message = [this](auto&& msg) { this->on_received_message(msg); };
      osc_packet_processor<decltype(on_message)>{on_message}(data, sz);
    });
  }

  osc_generic_bidir_protocol(
      network_context_ptr ctx, const outbound_socket_configuration& send_conf,
      const inbound_socket_configuration& recv_conf)
      : can_learn<ossia::net::protocol_base>{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , from_client{recv_conf, m_ctx->context}
      , to_client{send_conf, m_ctx->context}
  {
    from_client.open();
    to_client.connect();

    from_client.receive([this](const char* data, std::size_t sz) {
      if(!m_device)
        return;
      auto on_message = [this](auto&& msg) { this->on_received_message(msg); };
      osc_packet_processor<decltype(on_message)>{on_message}(data, sz);
    });
  }

  osc_generic_bidir_protocol(
      network_context_ptr ctx, const outbound_socket_configuration& send_conf)
      : can_learn<ossia::net::protocol_base>{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , to_client{send_conf, m_ctx->context}
  {
    to_client.connect();
  }

  osc_generic_bidir_protocol(
      network_context_ptr ctx, const inbound_socket_configuration& recv_conf)
      : can_learn<ossia::net::protocol_base>{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , from_client{recv_conf, m_ctx->context}
  {
    from_client.open();

    from_client.receive([this](const char* data, std::size_t sz) {
      if(!m_device)
        return;
      auto on_message = [this](auto&& msg) { this->on_received_message(msg); };
      osc_packet_processor<decltype(on_message)>{on_message}(data, sz);
    });
  }

  osc_generic_bidir_protocol(
      network_context_ptr ctx, const send_fd_configuration& send_conf)
      : can_learn<ossia::net::protocol_base>{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , to_client{send_conf, m_ctx->context}
  {
    to_client.connect();
  }

  osc_generic_bidir_protocol(
      network_context_ptr ctx, const receive_fd_configuration& recv_conf)
      : can_learn<ossia::net::protocol_base>{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , from_client{recv_conf, m_ctx->context}
  {
    from_client.open();

    from_client.receive([this](const char* data, std::size_t sz) {
      if(!m_device)
        return;
      auto on_message = [this](auto&& msg) { this->on_received_message(msg); };
      osc_packet_processor<decltype(on_message)>{on_message}(data, sz);
    });
  }

  osc_generic_bidir_protocol(const osc_generic_bidir_protocol&) = delete;
  osc_generic_bidir_protocol(osc_generic_bidir_protocol&&) = delete;
  osc_generic_bidir_protocol& operator=(const osc_generic_bidir_protocol&) = delete;
  osc_generic_bidir_protocol& operator=(osc_generic_bidir_protocol&&) = delete;

  ~osc_generic_bidir_protocol() override = default;

  bool update(ossia::net::node_base& node_base) override { return false; }

  bool pull(ossia::net::parameter_base& parameter_base) override { return false; }

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override
  {
    if constexpr(!std::is_same_v<RecvSocket, ossia::net::null_socket>)
    {
      return OscMode::observe(*this, parameter_base, enable);
    }
    else
    {
      return false;
    }
  }

  bool echo_incoming_message(
      const message_origin_identifier& id, const parameter_base& addr,
      const value& val) override
  {
    if constexpr(!std::is_same_v<SendSocket, ossia::net::null_socket>)
    {
      // FIXME bundling ?
      return OscMode::echo_incoming_message(*this, id, addr, val);
    }
    else
    {
      return false;
    }
  }

  bool push(const ossia::net::parameter_base& addr, const ossia::value& v) override
  {
    if constexpr(!std::is_same_v<SendSocket, ossia::net::null_socket>)
    {
      if constexpr(bundled)
        return OscMode::push_bundle(*this, writer(), addr, v);
      else
        return OscMode::push(*this, addr, v);
    }
    else
    {
      return false;
    }
  }

  bool push(const ossia::net::parameter_base& addr, ossia::value&& v) override
  {
    if constexpr(!std::is_same_v<SendSocket, ossia::net::null_socket>)
    {
      if constexpr(bundled)
        return OscMode::push_bundle(*this, writer(), addr, std::move(v));
      else
        return OscMode::push(*this, addr, std::move(v));
    }
    else
    {
      return false;
    }
  }

  bool push_raw(const ossia::net::full_parameter_data& addr) override
  {
    if constexpr(!std::is_same_v<SendSocket, ossia::net::null_socket>)
    {
      if constexpr(bundled)
        return OscMode::push_bundle(*this, writer(), addr);
      else
        return OscMode::push_raw(*this, addr);
    }
    else
    {
      return false;
    }
  }

  bool push_bundle(const std::vector<const parameter_base*>& addresses) override
  {
    if constexpr(!std::is_same_v<SendSocket, ossia::net::null_socket>)
    {
      return OscMode::push_bundle(*this, writer(), addresses);
    }
    else
    {
      return false;
    }
  }

  bool push_bundle(std::span<ossia::bundle_element> addresses) override
  {
    if constexpr(!std::is_same_v<SendSocket, ossia::net::null_socket>)
    {
      return OscMode::push_bundle(*this, writer(), addresses);
    }
    else
    {
      return false;
    }
  }

  bool push_bundle_bounded(std::span<ossia::bundle_element> addresses) override
  {
    if constexpr(!std::is_same_v<SendSocket, ossia::net::null_socket>)
    {
      return OscMode::push_bundle_bounded(*this, writer(), addresses);
    }
    else
    {
      return false;
    }
  }

  bool
  push_raw_bundle(const std::vector<ossia::net::full_parameter_data>& addresses) override
  {
    if constexpr(!std::is_same_v<SendSocket, ossia::net::null_socket>)
    {
      return OscMode::push_bundle(*this, writer(), addresses);
    }
    else
    {
      return false;
    }
  }

  void on_received_message(const oscpack::ReceivedMessage& m)
  {
    if constexpr(!std::is_same_v<RecvSocket, ossia::net::null_socket>)
    {
      return OscMode::on_received_message(*this, m);
    }
  }

  void set_device(ossia::net::device_base& dev) override { m_device = &dev; }

  auto writer() noexcept { return writer_type{to_client}; }

  using ossia::net::protocol_base::m_logger;
  ossia::net::network_context_ptr m_ctx;
  message_origin_identifier m_id;
  listened_parameters m_listening;

  ossia::net::device_base* m_device{};

  RecvSocket from_client;
  SendSocket to_client;
};

template <typename OscMode, typename Socket>
class osc_generic_server_protocol final : public can_learn<ossia::net::protocol_base>
{
public:
  using osc_configuration = typename OscMode::osc_configuration;
  static constexpr bool bundled = requires { typename osc_configuration::bundled; };
  using socket_type = Socket;
  using writer_type = socket_writer<socket_type>;

  template <typename Configuration>
    requires(requires(Configuration conf) { Socket{conf, network_context_ptr{}}; })
  osc_generic_server_protocol(network_context_ptr ctx, const Configuration& conf)
      : can_learn<ossia::net::protocol_base>{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , m_server{conf, m_ctx}
  {
    init();
  }

  void init()
  {
    m_server.listen([this](const unsigned char* data, std::size_t sz) {
      auto on_message = [this](auto&& msg) { this->on_received_message(msg); };
      osc_packet_processor<decltype(on_message)>{on_message}((const char*)data, sz);
    });
  }

  osc_generic_server_protocol(const osc_generic_server_protocol&) = delete;
  osc_generic_server_protocol(osc_generic_server_protocol&&) = delete;
  osc_generic_server_protocol& operator=(const osc_generic_server_protocol&) = delete;
  osc_generic_server_protocol& operator=(osc_generic_server_protocol&&) = delete;

  ~osc_generic_server_protocol() override = default;

  bool update(ossia::net::node_base& node_base) override { return false; }

  bool pull(ossia::net::parameter_base& parameter_base) override { return false; }

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override
  {
    return OscMode::observe(*this, parameter_base, enable);
  }

  bool echo_incoming_message(
      const message_origin_identifier& id, const parameter_base& addr,
      const value& val) override
  {
    return OscMode::echo_incoming_message(*this, id, addr, val);
  }

  bool push(const ossia::net::parameter_base& addr, const ossia::value& v) override
  {
    if constexpr(bundled)
      return OscMode::push_bundle(*this, writer(), addr, v);
    else
      return OscMode::push(*this, addr, v);
  }

  bool push(const ossia::net::parameter_base& addr, ossia::value&& v) override
  {
    if constexpr(bundled)
      return OscMode::push_bundle(*this, writer(), addr, std::move(v));
    else
      return OscMode::push(*this, addr, std::move(v));
  }

  bool push_raw(const ossia::net::full_parameter_data& addr) override
  {
    if constexpr(bundled)
      return OscMode::push_bundle(*this, writer(), addr);
    else
      return OscMode::push_raw(*this, addr);
  }

  bool push_bundle(const std::vector<const parameter_base*>& addresses) override
  {
    return OscMode::push_bundle(*this, writer(), addresses);
  }

  bool push_bundle(std::span<ossia::bundle_element> addresses) override
  {
    return OscMode::push_bundle(*this, writer(), addresses);
  }

  bool push_bundle_bounded(std::span<ossia::bundle_element> addresses) override
  {
    return OscMode::push_bundle_bounded(*this, writer(), addresses);
  }

  bool
  push_raw_bundle(const std::vector<ossia::net::full_parameter_data>& addresses) override
  {
    return OscMode::push_bundle(*this, writer(), addresses);
  }

  void on_received_message(const oscpack::ReceivedMessage& m)
  {
    return OscMode::on_received_message(*this, m);
  }

  void set_device(ossia::net::device_base& dev) override { m_device = &dev; }

  auto writer() noexcept { return writer_type{m_server}; }

  using ossia::net::protocol_base::m_logger;
  ossia::net::network_context_ptr m_ctx;
  message_origin_identifier m_id;
  listened_parameters m_listening;

  ossia::net::device_base* m_device{};

  Socket m_server;
};

template <typename OscMode, typename Socket>
class osc_generic_client_protocol : public can_learn<ossia::net::protocol_base>
{
public:
  using osc_configuration = typename OscMode::osc_configuration;
  static constexpr bool bundled = requires { typename osc_configuration::bundled; };
  using socket_type = Socket;
  using writer_type = socket_writer<socket_type>;

  template <typename Configuration>
  osc_generic_client_protocol(network_context_ptr ctx, const Configuration& conf)
      : can_learn<ossia::net::protocol_base>{flags{SupportsMultiplex}}
      , m_ctx{std::move(ctx)}
      , m_id{*this}
      , m_client{conf, m_ctx->context}
  {
    init();
  }

  void init()
  {
    m_client.on_open.connect(this->on_connection_open);
    m_client.on_close.connect(this->on_connection_closed);
    m_client.on_fail.connect(this->on_connection_failure);

    m_client.connect();
    m_client.receive([this](const unsigned char* data, std::size_t sz) {
      auto on_message = [this](auto&& msg) { this->on_received_message(msg); };
      osc_packet_processor<decltype(on_message)>{on_message}((const char*)data, sz);
    });
  }

  osc_generic_client_protocol(const osc_generic_client_protocol&) = delete;
  osc_generic_client_protocol(osc_generic_client_protocol&&) = delete;
  osc_generic_client_protocol& operator=(const osc_generic_client_protocol&) = delete;
  osc_generic_client_protocol& operator=(osc_generic_client_protocol&&) = delete;

  ~osc_generic_client_protocol() override = default;

  bool update(ossia::net::node_base& node_base) override { return false; }

  bool pull(ossia::net::parameter_base& parameter_base) override { return false; }

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override
  {
    return OscMode::observe(*this, parameter_base, enable);
  }

  bool echo_incoming_message(
      const message_origin_identifier& id, const parameter_base& addr,
      const value& val) override
  {
    return OscMode::echo_incoming_message(*this, id, addr, val);
  }

  bool push(const ossia::net::parameter_base& addr, const ossia::value& v) override
  {
    if constexpr(bundled)
      return OscMode::push_bundle(*this, writer(), addr, v);
    else
      return OscMode::push(*this, addr, v);
  }

  bool push(const ossia::net::parameter_base& addr, ossia::value&& v) override
  {
    if constexpr(bundled)
      return OscMode::push_bundle(*this, writer(), addr, std::move(v));
    else
      return OscMode::push(*this, addr, std::move(v));
  }

  bool push_raw(const ossia::net::full_parameter_data& addr) override
  {
    if constexpr(bundled)
      return OscMode::push_bundle(*this, writer(), addr);
    else
      return OscMode::push_raw(*this, addr);
  }

  bool push_bundle(const std::vector<const parameter_base*>& addresses) override
  {
    return OscMode::push_bundle(*this, writer(), addresses);
  }

  bool push_bundle(std::span<ossia::bundle_element> addresses) override
  {
    return OscMode::push_bundle(*this, writer(), addresses);
  }

  bool push_bundle_bounded(std::span<ossia::bundle_element> addresses) override
  {
    return OscMode::push_bundle_bounded(*this, writer(), addresses);
  }

  bool
  push_raw_bundle(const std::vector<ossia::net::full_parameter_data>& addresses) override
  {
    return OscMode::push_bundle(*this, writer(), addresses);
  }

  void on_received_message(const oscpack::ReceivedMessage& m)
  {
    return OscMode::on_received_message(*this, m);
  }

  void set_device(ossia::net::device_base& dev) override { m_device = &dev; }

  auto writer() noexcept { return writer_type{m_client}; }

  bool connected() const noexcept override { return m_client.connected(); }

  void connect() override { return m_client.connect(); }

  using ossia::net::protocol_base::m_logger;
  ossia::net::network_context_ptr m_ctx;
  message_origin_identifier m_id;
  listened_parameters m_listening;

  ossia::net::device_base* m_device{};

  Socket m_client;
};
}
