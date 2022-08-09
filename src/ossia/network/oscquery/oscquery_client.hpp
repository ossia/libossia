#pragma once
#include <ossia/detail/mutex.hpp>
#include <ossia/detail/string_map.hpp>
#include <ossia/network/common/network_logger.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/oscquery/detail/outbound_visitor.hpp>
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/sockets/websocket_server.hpp>

namespace osc
{
template <typename T>
class sender;
}
namespace ossia
{
namespace net
{
class parameter_base;
}
namespace oscquery
{
struct oscquery_client
{
  ossia::net::websocket_server::connection_handler connection;
  mutex_t listeningMutex;
  string_map<ossia::net::parameter_base*> listening TS_GUARDED_BY(listeningMutex);

  std::string client_ip;
  std::unique_ptr<osc::sender<oscquery::osc_outbound_visitor>> sender;
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
  open_osc_sender(const ossia::oscquery::oscquery_server_protocol& proto, uint16_t port)
  {
    sender = std::make_unique<osc::sender<oscquery::osc_outbound_visitor>>(
        proto.get_logger(), client_ip, port);
  }
};
}
}
