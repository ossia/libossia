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
}

namespace ossia::net
{
struct osc_protocol_common;
class udp_socket;
struct network_context;
using network_context_ptr = std::shared_ptr<network_context>;

class OSSIA_EXPORT osc_udp_protocol final
    : public ossia::net::protocol_base
{
  friend struct osc_protocol_common;
public:
  using socket_type = udp_socket;
  osc_udp_protocol(
      network_context_ptr ctx,
      std::string_view local_host,  uint16_t local_port,
      std::string_view remote_host, uint16_t remote_port);

  osc_udp_protocol(const osc_udp_protocol&) = delete;
  osc_udp_protocol(osc_udp_protocol&&) = delete;
  osc_udp_protocol& operator=(const osc_udp_protocol&) = delete;
  osc_udp_protocol& operator=(osc_udp_protocol&&) = delete;

  ~osc_udp_protocol() override;

  bool learning() const;
  osc_udp_protocol& set_learning(bool);

  bool update(ossia::net::node_base& node_base) override;

  bool pull(ossia::net::parameter_base& parameter_base) override;

  bool push(const ossia::net::parameter_base& parameter_base, const ossia::value& v) override;
  bool push(const ossia::net::parameter_base& parameter_base, ossia::value&& v) override;
  bool push_raw(const ossia::net::full_parameter_data& parameter_base) override;
  bool push_bundle(const std::vector<const ossia::net::parameter_base*>&) override;
  bool push_raw_bundle(const std::vector<full_parameter_data>&) override;

  bool observe(ossia::net::parameter_base& parameter_base, bool enable) override;

  bool echo_incoming_message(const message_origin_identifier& id, const parameter_base& addr, const value& val) override;

private:
  void on_received_message(const oscpack::ReceivedMessage& m);
  void on_learn(const oscpack::ReceivedMessage& m);
  void set_device(ossia::net::device_base& dev) override;

  struct impl;
  impl* m_impl{};

  message_origin_identifier m_id;
  listened_parameters m_listening;

  ossia::net::device_base* m_device{};
  std::string m_localHost;
  std::string m_remoteHost;
  uint16_t m_localPort{};
  uint16_t m_remotePort{};

  std::atomic_bool m_learning{};
};
}
