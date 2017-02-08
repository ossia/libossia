#pragma once
#include <ossia/network/oscquery/detail/server.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <hopscotch_map.h>

namespace ossia
{
namespace oscquery
{

struct oscquery_client
{
  websocket_server::connection_handler connection;
  std::mutex listeningMutex;
  tsl::hopscotch_map<std::string, ossia::net::address_base*> listening;

  std::unique_ptr<osc::sender> sender;

public:
  oscquery_client() = default;
  oscquery_client(oscquery_client&& other):
    connection{std::move(other.connection)},
    listening{std::move(other.listening)},
    sender{std::move(other.sender)}
  {
    //FIXME http://stackoverflow.com/a/29988626/1495627
  }

  oscquery_client& operator=(oscquery_client&& other)
  {
    connection = std::move(other.connection);
    listening = std::move(other.listening);
    sender = std::move(other.sender);
    return *this;
  }

  oscquery_client(websocket_server::connection_handler h):
    connection{std::move(h)}
  {

  }

  void start_listen(std::string path, ossia::net::address_base* addr)
  {
    if(addr)
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

  bool operator==(
      const websocket_server::connection_handler& h) const
  {
    return !connection.expired() && connection.lock() == h.lock();
  }
};

}
}
