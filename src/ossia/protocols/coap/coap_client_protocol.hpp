#pragma once
#include <ossia/detail/audio_spin_mutex.hpp>
#include <ossia/detail/timer.hpp>
#include <ossia/detail/variant.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/configuration.hpp>

#include <boost/asio/cancellation_signal.hpp>
#include <boost/asio/strand.hpp>
#include <boost/unordered/concurrent_flat_map.hpp>

namespace ossia::net
{
struct coap_session;
using strand_type
    = decltype(boost::asio::make_strand(std::declval<boost::asio::io_context&>()));
struct coap_client_configuration
{
  ossia::variant<udp_configuration, tcp_client_configuration, ws_client_configuration>
      transport;
};
struct coap_client;
class OSSIA_EXPORT coap_client_protocol
    : public ossia::net::protocol_base
    , public Nano::Observer
{
public:
  explicit coap_client_protocol(
      ossia::net::network_context_ptr, const coap_client_configuration& conf);
  ~coap_client_protocol();

  bool pull(parameter_base&) override;
  bool push(const parameter_base&, const value& v) override;
  bool push_raw(const full_parameter_data&) override;
  bool observe(parameter_base&, bool) override;
  bool update(node_base& node_base) override;
  void set_device(device_base& dev) override;
  void stop() override;

private:
  void request_namespace(ossia::net::node_base& root, std::string_view req);
  void parse_namespace(ossia::net::node_base& dev, std::string_view data);

  ossia::net::network_context_ptr m_context;
  ossia::net::device_base* m_device{};
  coap_client_configuration m_conf{};

  std::unique_ptr<coap_client> m_client;
  boost::unordered::concurrent_flat_map<std::string, std::shared_ptr<coap_session>>
      m_topics;

  strand_type m_strand;
  std::string m_host;

  ossia::timer m_timer;
};
}
