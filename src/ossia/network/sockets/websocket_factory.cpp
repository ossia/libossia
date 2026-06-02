#include <ossia/network/sockets/websocket_factory.hpp>

#if defined(__EMSCRIPTEN__)
#include <ossia/network/sockets/websocket_client_emscripten.hpp>
#else
#include <ossia/network/sockets/websocket_client_beast.hpp>
#endif

namespace ossia::net
{

std::unique_ptr<websocket_client_interface> make_websocket_client(
    boost::asio::io_context& ctx, ws_client_message_handler handler,
    websocket_backend backend)
{
#if defined(__EMSCRIPTEN__)
  // Only the Emscripten backend is available in WebAssembly builds; the
  // browser handles `wss://` automatically.
  (void)backend;
  return std::make_unique<websocket_client_emscripten>(ctx, std::move(handler));
#else
  // The Socket.IO client lives in a different translation unit (and pulls
  // in boost::json), so we don't construct it from here. Callers that want
  // it should keep instantiating it directly. Anything else maps to Beast.
  (void)backend;
  return std::make_unique<websocket_client_beast>(ctx, std::move(handler));
#endif
}

}
