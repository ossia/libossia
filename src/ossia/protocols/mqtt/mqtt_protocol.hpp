#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context.hpp>

#include <boost/asio/cancellation_signal.hpp>
#include <boost/unordered/concurrent_flat_map.hpp>
namespace ossia::net
{
struct mqtt5_configuration
{
  std::string host;
  int port;
};

struct mqtt5_client;

class OSSIA_EXPORT mqtt5_protocol
    : public ossia::net::protocol_base
    , public Nano::Observer
{
public:
  explicit mqtt5_protocol(
      ossia::net::network_context_ptr, const mqtt5_configuration& conf);
  ~mqtt5_protocol();

  bool pull(parameter_base&) override;
  bool push(const parameter_base&, const value& v) override;
  bool push_raw(const full_parameter_data&) override;
  bool observe(parameter_base&, bool) override;
  bool update(node_base& node_base) override;
  void set_device(device_base& dev) override;

private:
  void on_new_param(const parameter_base& param);
  void on_removed_param(const parameter_base& param);
  void on_subscribe(const parameter_base& param);
  void on_unsubscribe(const parameter_base& param);

  ossia::net::network_context_ptr m_context;
  mqtt5_configuration m_conf{};
  std::unique_ptr<mqtt5_client> m_client;

  struct subscribe_state;
  boost::unordered::concurrent_flat_map<
      const ossia::net::parameter_base*, subscribe_state>
      m_subscriptions;
};
}
