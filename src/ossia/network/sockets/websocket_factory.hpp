#pragma once

#include <ossia/detail/config.hpp>
#include <ossia/network/sockets/websocket_client_interface.hpp>

#include <memory>
#include <string>

namespace boost::asio
{
class io_context;
}

namespace ossia::net
{

/// Backend selection for the websocket client factory.
///
/// auto: pick the best implementation for the current build:
///   - Emscripten target: native browser WebSocket via emscripten_websocket_*
///   - Otherwise: Boost.Beast (plaintext TCP only)
/// TLS (wss://) is not yet implemented in the Beast backend: a wss:// URI is
/// rejected with a failure rather than being silently downgraded to plaintext.
/// Other values force a specific implementation when available; if the
/// requested backend is not available, the factory falls back to `auto`.
enum class websocket_backend
{
  automatic,
  beast,
  emscripten,
  socketio,
};

/// Construct a websocket client implementing websocket_client_interface.
///
/// The implementation is chosen based on the active build (Emscripten vs
/// native), the requested backend, and the URL scheme. The returned object
/// can be `connect()`-ed to a `ws://` or `http://` URI. Secure schemes
/// (`wss://`, `https://`) are not yet supported by the Beast backend and the
/// connection will fail rather than fall back to plaintext.
OSSIA_EXPORT
std::unique_ptr<websocket_client_interface> make_websocket_client(
    boost::asio::io_context& ctx, ws_client_message_handler handler,
    websocket_backend backend = websocket_backend::automatic);

}
