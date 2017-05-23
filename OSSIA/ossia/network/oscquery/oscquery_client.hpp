#pragma once
#include <ossia/network/oscquery/detail/server.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/detail/string_map.hpp>
#include <ossia/detail/mutex.hpp>

namespace ossia
{
namespace oscquery
{
struct osc_outbound_visitor
{
  public:
    oscpack::OutboundPacketStream& p;
    void operator()(ossia::impulse) const
    {
    }
    void operator()(int32_t i) const
    {
      p << int32_t(i);
    }
    void operator()(float f) const
    {
      p << float(f);
    }
    void operator()(bool b) const
    {
      p << bool(b);
    }
    void operator()(char c) const
    {
      p << char(c);
    }
    void operator()(const std::string& str) const
    {
      p << (ossia::string_view)str;
    }
    void operator()(ossia::vec2f vec) const
    {
      p << vec[0] << vec[1];
    }
    void operator()(ossia::vec3f vec) const
    {
      p << vec[0] << vec[1] << vec[2];
    }
    void operator()(ossia::vec4f vec) const
    {
      p << vec[0] << vec[1] << vec[2] << vec[3];
    }
    void operator()(const std::vector<ossia::value>& t) const
    {
      for (const auto& val : t)
      {
        val.apply(*this);
      }
    }
    void operator()() const
    {
    }
};

struct oscquery_client
{
  websocket_server::connection_handler connection;
  mutex_t listeningMutex;
  string_map<ossia::net::address_base*> listening;

  std::string client_ip;
  std::unique_ptr<osc::sender<oscquery::osc_outbound_visitor>> sender;

public:
  oscquery_client() = default;
  oscquery_client(oscquery_client&& other):
    connection{std::move(other.connection)},
    listening{std::move(other.listening)},
    client_ip{std::move(other.client_ip)},
    sender{std::move(other.sender)}
  {
    //FIXME http://stackoverflow.com/a/29988626/1495627
  }

  oscquery_client& operator=(oscquery_client&& other)
  {
    connection = std::move(other.connection);
    listening = std::move(other.listening);
    client_ip = std::move(other.client_ip);
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

  void openOSCSender(const ossia::net::network_logger& l,  uint16_t port)
  {
    sender = std::make_unique<osc::sender<oscquery::osc_outbound_visitor>>(l, client_ip, port);
  }
};

}
}
