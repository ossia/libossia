#pragma once
#if defined(__EMSCRIPTEN__)

#include <ossia/detail/config.hpp>
#include <ossia/network/sockets/websocket_client_interface.hpp>

#include <boost/asio/io_context.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

namespace ossia::net
{

/// WebSocket client implementation for Emscripten / WebAssembly builds.
///
/// Wraps the browser's native WebSocket API exposed through
/// `<emscripten/websocket.h>`. The browser handles `wss://` automatically
/// using its own trust store, so no TLS configuration is needed (or in
/// fact, possible) on this backend.
///
/// All event callbacks are marshalled back into the supplied io_context
/// via boost::asio::post so that user code observes them on the libossia
/// network thread, mirroring the Beast implementation.
class OSSIA_EXPORT websocket_client_emscripten final
    : public websocket_client_interface
{
public:
  explicit websocket_client_emscripten(boost::asio::io_context& ctx);
  websocket_client_emscripten(
      boost::asio::io_context& ctx, ws_client_message_handler handler);

  ~websocket_client_emscripten() override;

  void connect(const std::string& uri) override;
  void connect_and_run(const std::string& uri) override;
  void stop() override;
  bool connected() const override;

  void send_message(const std::string& request) override;
  void send_message(const rapidjson::StringBuffer& request) override;
  void send_binary_message(std::string_view request) override;

private:
  /// Per-instance shared state. Held by a shared_ptr so that the C-style
  /// callbacks (which receive a raw `void*` userData) can safely outlive
  /// the public object even if the destructor races with an in-flight
  /// browser event.
  struct state
  {
    state(boost::asio::io_context& c, ws_client_message_handler h)
        : context{c}
        , on_message{std::move(h)}
    {
    }

    boost::asio::io_context& context;
    ws_client_message_handler on_message;
    Nano::Signal<void()>* on_open{};
    Nano::Signal<void()>* on_close{};
    Nano::Signal<void()>* on_fail{};
    std::atomic_bool active{true};
    std::atomic_bool open{false};
    int socket{0}; // EMSCRIPTEN_WEBSOCKET_T
    std::mutex mutex;
  };

  std::shared_ptr<state> m_state;
};

}

#endif
