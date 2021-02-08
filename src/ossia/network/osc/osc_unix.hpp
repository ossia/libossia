#pragma once
#include <ossia/detail/mutex.hpp>
#include <ossia/network/base/listening.hpp>
#include <ossia/network/base/protocol.hpp>

#include <tsl/hopscotch_map.h>

#include <atomic>
#include <string>

namespace oscpack
{
class ReceivedMessage;
class IpEndpointName;
}
namespace osc
{
template <typename T>
class sender;
class receiver;
}
namespace ossia
{
namespace net
{
struct osc_protocol_common;
struct osc_protocol_client;
struct osc_protocol_server;
class unix_socket;
struct network_context;
using network_context_ptr = std::shared_ptr<network_context>;

class OSSIA_EXPORT osc_unix_protocol : public ossia::net::protocol_base
{
  friend struct osc_protocol_common;
public:
  using socket_type = unix_socket;
  osc_unix_protocol(
      network_context_ptr ctx,
      std::string_view socket_name);

  osc_unix_protocol(const osc_unix_protocol&) = delete;
  osc_unix_protocol(osc_unix_protocol&&) = delete;
  osc_unix_protocol& operator=(const osc_unix_protocol&) = delete;
  osc_unix_protocol& operator=(osc_unix_protocol&&) = delete;

  ~osc_unix_protocol() override;

  bool learning() const;
  osc_unix_protocol& set_learning(bool);

  bool update(ossia::net::node_base& node_base) override;
  bool pull(ossia::net::parameter_base& parameter_base) override;
  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override;
  bool echo_incoming_message(
      const message_origin_identifier& id, const parameter_base& addr, const value& val) override;

protected:
  void on_received_message(const oscpack::ReceivedMessage& m);
  void on_learn(const oscpack::ReceivedMessage& m);
  void set_device(ossia::net::device_base& dev) override;

  struct impl;
  impl* m_impl{};

  ossia::net::network_context_ptr m_ctx;
  message_origin_identifier m_id;
  listened_parameters m_listening;

  ossia::net::device_base* m_device{};
  std::string m_localSocket;
  std::string m_remoteSocket;

  std::atomic_bool m_learning{};
};

class OSSIA_EXPORT osc_unix_server final
    : public osc_unix_protocol
{
public:
  osc_unix_server(
      network_context_ptr ctx,
      std::string_view socket_name);
  bool push(const ossia::net::parameter_base& parameter_base, const ossia::value& v) override;
  bool push(const ossia::net::parameter_base& parameter_base, ossia::value&& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw_bundle(const std::vector<full_parameter_data>&) override;
};

class OSSIA_EXPORT osc_unix_client final
    : public osc_unix_protocol
{
public:
  osc_unix_client(
      network_context_ptr ctx,
      std::string_view socket_name);

  bool push(const ossia::net::parameter_base& parameter_base, const ossia::value& v) override;
  bool push(const ossia::net::parameter_base& parameter_base, ossia::value&& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw_bundle(const std::vector<full_parameter_data>&) override;
};
}
}
