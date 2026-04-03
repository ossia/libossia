#pragma once

#include <ossia/detail/config.hpp>
#include <ossia/network/sockets/websocket_common.hpp>
#include <ossia/network/sockets/websocket_reply.hpp>

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace rapidjson
{
class CrtAllocator;
template <typename BaseAllocator>
class MemoryPoolAllocator;
template <typename Encoding, typename Allocator>
class GenericStringBuffer;
template <typename CharType>
struct UTF8;
using StringBuffer = GenericStringBuffer<UTF8<char>, CrtAllocator>;
}

namespace ossia::net
{

/// Callback: (handle, opcode, payload) → reply
using ws_server_message_handler = std::function<
    server_reply(const ws_connection_handle&, ws_opcode, const std::string&)>;

using ws_open_handler = std::function<void(ws_connection_handle)>;
using ws_close_handler = std::function<void(ws_connection_handle)>;

/// Abstract interface for a WebSocket + HTTP server.
///
/// Both plain beast and Socket.IO server implementations
/// derive from this interface.
class OSSIA_EXPORT websocket_server_interface
{
public:
  virtual ~websocket_server_interface();

  virtual void listen(uint16_t port) = 0;
  virtual void run() = 0;
  virtual void stop() = 0;
  virtual void close(ws_connection_handle hdl) = 0;

  virtual void set_open_handler(ws_open_handler) = 0;
  virtual void set_close_handler(ws_close_handler) = 0;

  /// The message handler is called for both WebSocket messages and HTTP requests.
  /// For HTTP requests, the opcode is ws_opcode::text and the payload is the URI.
  /// The returned server_reply is sent back to the client.
  virtual void set_message_handler(ws_server_message_handler) = 0;

  /// Get the remote IP address of a connected client (e.g. "192.168.1.10").
  virtual std::string get_remote_ip(const ws_connection_handle& hdl) = 0;

  /// Get the remote endpoint string (e.g. "192.168.1.10:54321").
  virtual std::string get_remote_endpoint(const ws_connection_handle& hdl) = 0;

  /// Get the local IP address as seen by a connected client.
  virtual std::string get_local_ip(const ws_connection_handle& hdl) = 0;

  virtual void send_message(ws_connection_handle hdl, const std::string& message) = 0;
  virtual void
  send_message(ws_connection_handle hdl, const server_reply& message) = 0;
  virtual void
  send_message(ws_connection_handle hdl, const rapidjson::StringBuffer& message) = 0;
  virtual void
  send_binary_message(ws_connection_handle hdl, std::string_view message) = 0;
};

}
