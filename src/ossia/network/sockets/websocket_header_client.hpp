#pragma once

// A minimal, fully header-only WebSocket client.
//
// Unlike websocket_client_beast (whose implementation lives in a .cpp and thus
// requires linking libossia), this client is entirely inline so it can be used
// by tiny standalone executables that consume ossia headers only
// (e.g. score's vst/vst3/clap puppets, which link `$<COMPILE_ONLY:ossia>` and
// must not depend on libossia.so).
//
// It supports exactly what those tools need: connect to a ws:// endpoint, fire
// on_open/on_fail/on_close, send text/binary frames and close. It is meant to
// be driven by a single-threaded io_context (the caller runs ctx.run()), so
// sends are issued synchronously from that same thread.

#include <ossia/network/sockets/websocket_common.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <nano_signal_slot.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace ossia::net
{

class websocket_simple_header_client
{
public:
  explicit websocket_simple_header_client(boost::asio::io_context& ctx)
      : m_resolver{boost::asio::make_strand(ctx)}
      , m_ws{boost::asio::make_strand(ctx)}
  {
  }

  void connect(std::string_view uri)
  {
    auto parsed = parse_websocket_uri(uri);
    m_host = parsed.host;
    m_path = parsed.path;
    m_resolver.async_resolve(
        parsed.host, parsed.port,
        [this](
            boost::beast::error_code ec,
            boost::asio::ip::tcp::resolver::results_type results) {
      on_resolve(ec, std::move(results));
    });
  }

  void send_message(std::string_view msg)
  {
    if(!m_open)
      return;
    boost::beast::error_code ec;
    m_ws.text(true);
    m_ws.write(boost::asio::buffer(msg.data(), msg.size()), ec);
  }

  void send_binary_message(std::string_view msg)
  {
    if(!m_open)
      return;
    boost::beast::error_code ec;
    m_ws.binary(true);
    m_ws.write(boost::asio::buffer(msg.data(), msg.size()), ec);
  }

  void close()
  {
    m_open = false;
    boost::beast::error_code ec;
    auto& tcp = boost::beast::get_lowest_layer(m_ws);
    tcp.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    tcp.socket().close(ec);
  }

  bool connected() const { return m_open; }

  Nano::Signal<void()> on_open;
  Nano::Signal<void()> on_close;
  Nano::Signal<void()> on_fail;

private:
  void on_resolve(
      boost::beast::error_code ec,
      boost::asio::ip::tcp::resolver::results_type results)
  {
    if(ec)
    {
      on_fail();
      return;
    }
    boost::beast::get_lowest_layer(m_ws).async_connect(
        results,
        [this](
            boost::beast::error_code ec,
            boost::asio::ip::tcp::resolver::results_type::endpoint_type) {
      on_connect(ec);
    });
  }

  void on_connect(boost::beast::error_code ec)
  {
    if(ec)
    {
      on_fail();
      return;
    }
    boost::beast::get_lowest_layer(m_ws).expires_never();
    m_ws.async_handshake(
        m_host, m_path, [this](boost::beast::error_code ec) { on_handshake(ec); });
  }

  void on_handshake(boost::beast::error_code ec)
  {
    if(ec)
    {
      on_fail();
      return;
    }
    m_open = true;
    on_open();
    do_read();
  }

  void do_read()
  {
    // We don't process incoming frames, but keep a read in flight so the
    // io_context stays alive and connection errors surface as on_close.
    m_ws.async_read(
        m_buffer, [this](boost::beast::error_code ec, std::size_t) {
      if(ec)
      {
        if(m_open)
        {
          m_open = false;
          on_close();
        }
        return;
      }
      m_buffer.clear();
      do_read();
    });
  }

  boost::asio::ip::tcp::resolver m_resolver;
  boost::beast::websocket::stream<boost::beast::tcp_stream> m_ws;
  boost::beast::flat_buffer m_buffer;
  std::string m_host;
  std::string m_path;
  bool m_open{false};
};

}
