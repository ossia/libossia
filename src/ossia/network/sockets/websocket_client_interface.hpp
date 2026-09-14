#pragma once

#include <ossia/detail/config.hpp>
#include <ossia/network/sockets/websocket_common.hpp>

#include <nano_signal_slot.hpp>

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

/// Callback: (handle, opcode, payload)
using ws_client_message_handler = std::function<
    void(const ws_connection_handle&, ws_opcode, std::string&)>;

/// Abstract interface for a WebSocket client.
///
/// Both plain beast and Socket.IO client implementations
/// derive from this interface.
class OSSIA_EXPORT websocket_client_interface
{
public:
  virtual ~websocket_client_interface();

  /// Start an async connection to the given URI.
  virtual void connect(const std::string& uri) = 0;

  /// Connect and block until the connection closes or fails.
  virtual void connect_and_run(const std::string& uri) = 0;

  virtual void stop() = 0;
  virtual bool connected() const = 0;

  virtual void send_message(const std::string& request) = 0;
  virtual void send_message(const rapidjson::StringBuffer& request) = 0;
  virtual void send_binary_message(std::string_view request) = 0;

  Nano::Signal<void()> on_open;
  Nano::Signal<void()> on_close;
  Nano::Signal<void()> on_fail;
};

}
