#pragma once
#include <ossia/detail/mutex.hpp>
#include <ossia/detail/string_map.hpp>
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/sockets/websocket_server.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>

namespace ossia::oscquery_asio
{
struct oscquery_client
{
  ossia::net::websocket_server::connection_handler connection;
  mutex_t listeningMutex;
  string_map<ossia::net::parameter_base*> listening TS_GUARDED_BY(listeningMutex);

  std::string client_ip;
  std::unique_ptr<ossia::net::udp_send_socket> osc_socket;
  int remote_sender_port{};

public:
  oscquery_client() = delete;
  oscquery_client(const oscquery_client& other) noexcept = delete;
  oscquery_client& operator=(const oscquery_client& other) noexcept = delete;
  oscquery_client(oscquery_client&& other) noexcept = delete;
  oscquery_client& operator=(oscquery_client&& other) noexcept = delete;

  explicit oscquery_client(ossia::net::websocket_server::connection_handler h)
      : connection{std::move(h)}
  {
  }

  void start_listen(std::string path, ossia::net::parameter_base* addr)
  {
    if(addr)
    {
      std::lock_guard lck{listeningMutex};
      listening.insert(std::make_pair(std::move(path), addr));
    }
  }

  void stop_listen(const std::string& path)
  {
    std::lock_guard lck{listeningMutex};
    listening.erase(path);
  }

  bool operator==(const ossia::net::websocket_server::connection_handler& h) const
  {
    return !connection.expired() && connection.lock() == h.lock();
  }

  void
  open_osc_sender(ossia::oscquery_asio::oscquery_server_protocol_base& proto, uint16_t port)
  {
    osc_socket = std::make_unique<ossia::net::udp_send_socket>(
        ossia::net::outbound_socket_configuration{.host = client_ip, .port = port},
        proto.m_context->context);
    osc_socket->connect();
  }
};

}
