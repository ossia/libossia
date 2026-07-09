#if defined(__EMSCRIPTEN__)
#include <ossia/network/sockets/websocket_client_emscripten.hpp>

#include <ossia/detail/json.hpp>
#include <ossia/detail/logger.hpp>

#include <boost/asio/post.hpp>

#include <emscripten/websocket.h>

#include <cstring>

namespace ossia::net
{

namespace
{
using state_t = websocket_client_emscripten::state;
using state_ptr = std::shared_ptr<state_t>;

// userData is a heap-allocated `shared_ptr<state>*` so callbacks can take a
// strong reference to state for the duration of any posted continuation.
state_ptr state_from(void* userData)
{
  if(!userData)
    return {};
  auto* sp = static_cast<state_ptr*>(userData);
  return *sp;
}

EM_BOOL on_open_cb(int, const EmscriptenWebSocketOpenEvent*, void* userData)
{
  auto sp = state_from(userData);
  if(!sp || !sp->active.load())
    return EM_TRUE;
  sp->open.store(true);
  boost::asio::post(sp->context, [sp] {
    if(sp->active.load() && sp->on_open)
      (*sp->on_open)();
  });
  return EM_TRUE;
}

EM_BOOL on_close_cb(int, const EmscriptenWebSocketCloseEvent*, void* userData)
{
  auto sp = state_from(userData);
  if(!sp || !sp->active.load())
    return EM_TRUE;
  sp->open.store(false);
  boost::asio::post(sp->context, [sp] {
    if(sp->active.load() && sp->on_close)
      (*sp->on_close)();
  });
  return EM_TRUE;
}

EM_BOOL on_error_cb(int, const EmscriptenWebSocketErrorEvent*, void* userData)
{
  auto sp = state_from(userData);
  if(!sp || !sp->active.load())
    return EM_TRUE;
  sp->open.store(false);
  boost::asio::post(sp->context, [sp] {
    if(sp->active.load() && sp->on_fail)
      (*sp->on_fail)();
  });
  return EM_TRUE;
}

EM_BOOL on_message_cb(
    int, const EmscriptenWebSocketMessageEvent* ev, void* userData)
{
  auto sp = state_from(userData);
  if(!sp || !sp->active.load() || !ev || !ev->data || ev->numBytes == 0)
    return EM_TRUE;

  // The browser reuses ev->data immediately after the callback returns:
  // copy into an owned buffer before posting.
  auto opcode = ev->isText ? ws_opcode::text : ws_opcode::binary;
  std::string payload(
      reinterpret_cast<const char*>(ev->data), ev->numBytes);

  boost::asio::post(
      sp->context,
      [sp, opcode, data = std::move(payload)]() mutable {
    if(!sp->active.load())
      return;
    if(sp->on_message)
    {
      ws_connection_handle hdl; // unused for client
      sp->on_message(hdl, opcode, data);
    }
  });
  return EM_TRUE;
}
}

websocket_client_emscripten::websocket_client_emscripten(
    boost::asio::io_context& ctx)
    : m_state{std::make_shared<state>(ctx, ws_client_message_handler{})}
{
  m_state->on_open = &this->on_open;
  m_state->on_close = &this->on_close;
  m_state->on_fail = &this->on_fail;
}

websocket_client_emscripten::websocket_client_emscripten(
    boost::asio::io_context& ctx, ws_client_message_handler handler)
    : m_state{std::make_shared<state>(ctx, std::move(handler))}
{
  m_state->on_open = &this->on_open;
  m_state->on_close = &this->on_close;
  m_state->on_fail = &this->on_fail;
}

websocket_client_emscripten::~websocket_client_emscripten()
{
  stop();
}

void websocket_client_emscripten::connect(const std::string& uri)
{
  if(!emscripten_websocket_is_supported())
  {
    ossia::logger().error(
        "websocket_client_emscripten: WebSockets not supported by browser");
    on_fail();
    return;
  }

  EmscriptenWebSocketCreateAttributes attrs;
  emscripten_websocket_init_create_attributes(&attrs);
  attrs.url = uri.c_str();
  attrs.protocols = nullptr;
  attrs.createOnMainThread = EM_TRUE;

  EMSCRIPTEN_WEBSOCKET_T sock = emscripten_websocket_new(&attrs);
  if(sock <= 0)
  {
    ossia::logger().error(
        "websocket_client_emscripten: failed to create socket ({})", (int)sock);
    on_fail();
    return;
  }

  {
    std::lock_guard lock{m_state->mutex};
    m_state->socket = sock;
  }

  // Heap-allocated shared_ptr<state> kept alive for the connection's
  // lifetime so that browser callbacks can take a strong reference even
  // if our public object is destroyed before all in-flight callbacks
  // complete. Intentionally never freed (16-byte leak per connection):
  // there is no way to synchronously drain pending browser events.
  auto* userdata = new state_ptr(m_state);
  emscripten_websocket_set_onopen_callback(sock, userdata, on_open_cb);
  emscripten_websocket_set_onclose_callback(sock, userdata, on_close_cb);
  emscripten_websocket_set_onerror_callback(sock, userdata, on_error_cb);
  emscripten_websocket_set_onmessage_callback(sock, userdata, on_message_cb);
}

void websocket_client_emscripten::connect_and_run(const std::string& uri)
{
  // The browser drives the event loop; there is nothing meaningful to "run"
  // and blocking the calling thread would freeze the page. Just connect.
  connect(uri);
}

void websocket_client_emscripten::stop()
{
  std::shared_ptr<state> st = m_state;
  if(!st)
    return;

  st->active.store(false);
  st->open.store(false);

  int sock = 0;
  {
    std::lock_guard lock{st->mutex};
    sock = st->socket;
    st->socket = 0;
  }

  if(sock > 0)
  {
    emscripten_websocket_close(sock, 1000, "going away");
    emscripten_websocket_delete(sock);
  }
}

bool websocket_client_emscripten::connected() const
{
  return m_state && m_state->open.load();
}

void websocket_client_emscripten::send_message(const std::string& request)
{
  if(!m_state || !m_state->open.load())
    return;
  std::lock_guard lock{m_state->mutex};
  if(m_state->socket > 0)
  {
    auto rc = emscripten_websocket_send_utf8_text(
        m_state->socket, request.c_str());
    if(rc < 0)
      ossia::logger().error(
          "websocket_client_emscripten: send_utf8_text failed ({})", (int)rc);
  }
}

void websocket_client_emscripten::send_message(
    const rapidjson::StringBuffer& request)
{
  if(!m_state || !m_state->open.load())
    return;
  std::lock_guard lock{m_state->mutex};
  if(m_state->socket > 0)
  {
    // rapidjson::StringBuffer is null-terminated.
    auto rc = emscripten_websocket_send_utf8_text(
        m_state->socket, request.GetString());
    if(rc < 0)
      ossia::logger().error(
          "websocket_client_emscripten: send_utf8_text failed ({})", (int)rc);
  }
}

void websocket_client_emscripten::send_binary_message(std::string_view request)
{
  if(!m_state || !m_state->open.load())
    return;
  std::lock_guard lock{m_state->mutex};
  if(m_state->socket > 0)
  {
    auto rc = emscripten_websocket_send_binary(
        m_state->socket, const_cast<char*>(request.data()),
        static_cast<uint32_t>(request.size()));
    if(rc < 0)
      ossia::logger().error(
          "websocket_client_emscripten: send_binary failed ({})", (int)rc);
  }
}

}

#endif
