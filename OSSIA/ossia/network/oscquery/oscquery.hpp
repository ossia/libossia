#pragma once
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/oscquery/detail/server.hpp>
#include <eggs/variant.hpp>
#include <readerwriterqueue.h>

#include <hopscotch_map.h>
#include <mutex>
#include <atomic>
namespace osc
{
class sender;
class receiver;
}

namespace oscpack
{
class ReceivedMessage;
class IpEndpointName;
}

namespace ossia
{
namespace oscquery
{
//! TODO instead of having the command in a key, have a "command": "PathAdded"
class oscquery_namespace_command
{
};

class oscquery_path_added_command
{
};
class oscquery_path_removed_command
{
};
class oscquery_path_changed_command
{
};
class oscquery_attributes_changed_command
{
};

class oscquery_multi_path_added_command
{
};
class oscquery_multi_path_removed_command
{
};
class oscquery_multi_path_changed_command
{
};
class oscquery_multi_attributes_changed_command
{
};

struct oscquery_client
{
  websocket_server::connection_handler connection;
  std::mutex listeningMutex;
  tsl::hopscotch_map<std::string, ossia::net::address_base*> listening;

  std::unique_ptr<osc::sender> sender;

  bool operator==(
      const websocket_server::connection_handler& h) const
  { return !connection.expired() && connection.lock() == h.lock(); }
};

using oscquery_command =
  eggs::variant<
    oscquery_namespace_command,

    oscquery_path_added_command,
    oscquery_path_removed_command,
    oscquery_path_changed_command,
    oscquery_attributes_changed_command,

    oscquery_multi_path_added_command,
    oscquery_multi_path_removed_command,
    oscquery_multi_path_changed_command,
    oscquery_multi_attributes_changed_command
>;

class OSSIA_EXPORT oscquery_server_protocol : public ossia::net::protocol_base
{
public:
  using connection_handler = websocket_server::connection_handler;
  oscquery_server_protocol(int16_t osc_port, int16_t ws_port);
  ~oscquery_server_protocol();

  bool pull(net::address_base&) override;
  std::future<void> pullAsync(net::address_base&) override;
  void request(net::address_base&) override;
  bool push(const net::address_base&) override;
  bool observe(net::address_base&, bool) override;
  bool observe_quietly(net::address_base&, bool) override;
  bool update(net::node_base& b) override;
  void setDevice(net::device_base& dev) override;
  ossia::net::device_base& getDevice() const { return *m_device; }

  auto& clients() const { return m_clients; }
private:
  void on_OSCMessage(
      const oscpack::ReceivedMessage& m,
      const oscpack::IpEndpointName& ip);

  void on_connectionOpen(connection_handler hdl);

  void on_connectionClosed(connection_handler hdl);

  // Exceptions here will be catched by the server
  // which will set appropriate error codes.
  rapidjson::StringBuffer on_WSrequest(
      connection_handler hdl,
      const std::string& message);


  int16_t m_oscPort{};
  int16_t m_wsPort{};

  moodycamel::ReaderWriterQueue<oscquery_command> m_commandQueue;

  std::unique_ptr<osc::receiver> m_oscServer;
  websocket_server m_websocketServer;

  // Listening status of the local software
  std::mutex m_listeningMutex;
  tsl::hopscotch_map<std::string, ossia::net::address_base*> m_listening;

  std::vector<oscquery_client> m_clients;

  ossia::net::device_base* m_device{};
};

class oscquery_mirror_protocol
{
public:
private:
  moodycamel::ReaderWriterQueue<oscquery_command> m_commandQueue;

  oscquery_client m_client;
  websocket_server m_server;

  ossia::net::device_base* mDevice{};
};
}
}
