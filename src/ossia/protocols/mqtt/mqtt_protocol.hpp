#pragma once
#include <ossia/detail/audio_spin_mutex.hpp>
#include <ossia/detail/variant.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/configuration.hpp>

#include <boost/asio/cancellation_signal.hpp>
#include <boost/asio/strand.hpp>
#include <boost/unordered/concurrent_flat_map.hpp>
namespace ossia::net
{
using strand_type
    = decltype(boost::asio::make_strand(std::declval<boost::asio::io_context&>()));

struct mqtt5_configuration
{
  ossia::variant<tcp_client_configuration, ws_client_configuration> transport;
};

struct mqtt5_client_base;

class OSSIA_EXPORT mqtt5_protocol
    : public ossia::net::can_learn<ossia::net::protocol_base>
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

  void set_learning(bool);
  void on_learn(const std::string& topic, const std::string& payload);
  void on_message(const std::string& topic, const std::string& payload);

  void stop() override;

private:
  void on_new_param(const parameter_base& param);
  void on_removed_param(const parameter_base& param);
  void on_subscribe(parameter_base& param);
  void on_unsubscribe(const parameter_base& param);

  ossia::net::network_context_ptr m_context;
  ossia::net::device_base* m_device{};
  mqtt5_configuration m_conf{};
  std::unique_ptr<mqtt5_client_base> m_client;

  struct subscribe_state;
  boost::unordered::concurrent_flat_map<std::string, ossia::net::parameter_base*>
      m_topics;
  std::shared_ptr<subscribe_state> m_root;

  strand_type m_strand;
};
}
