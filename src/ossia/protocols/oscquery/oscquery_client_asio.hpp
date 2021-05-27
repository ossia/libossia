#pragma once
#include <ossia/detail/mutex.hpp>
#include <ossia/detail/string_map.hpp>
#include <ossia/protocols/oscquery/oscquery_server_asio.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/sockets/udp_socket.hpp>
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>
#include <ossia/network/sockets/websocket_server.hpp>

namespace ossia::oscquery_asio
{
struct oscquery_client
{
  ossia::net::websocket_server::connection_handler connection;
  mutex_t listeningMutex;
  string_map<ossia::net::parameter_base*> listening;

  std::string client_ip;
  std::unique_ptr<ossia::net::udp_send_socket> osc_socket;
  int remote_sender_port{};

public:
  oscquery_client() = default;
  oscquery_client(oscquery_client&& other)
      : connection{std::move(other.connection)}
      , listening{std::move(other.listening)}
      , client_ip{std::move(other.client_ip)}
      , osc_socket{std::move(other.osc_socket)}
  {
    // FIXME http://stackoverflow.com/a/29988626/1495627
  }

  oscquery_client& operator=(oscquery_client&& other)
  {
    connection = std::move(other.connection);
    listening = std::move(other.listening);
    client_ip = std::move(other.client_ip);
    osc_socket = std::move(other.osc_socket);
    return *this;
  }

  oscquery_client(ossia::net::websocket_server::connection_handler h)
      : connection{std::move(h)}
  {
  }

  void start_listen(std::string path, ossia::net::parameter_base* addr)
  {
    if (addr)
    {
      listeningMutex.lock();
      listening.insert(std::make_pair(std::move(path), addr));
      listeningMutex.unlock();
    }
  }

  void stop_listen(const std::string& path)
  {
    listeningMutex.lock();
    listening.erase(path);
    listeningMutex.unlock();
  }

  bool operator==(const ossia::net::websocket_server::connection_handler& h) const
  {
    return !connection.expired() && connection.lock() == h.lock();
  }

  void open_osc_sender(ossia::oscquery_asio::oscquery_server_protocol& proto, uint16_t port)
  {
    osc_socket = std::make_unique<ossia::net::udp_send_socket>(ossia::net::socket_configuration{client_ip, port}, proto.m_context->context);
    osc_socket->connect();
  }

};

}
