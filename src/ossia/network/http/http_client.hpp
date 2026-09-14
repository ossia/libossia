#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/logger.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace ossia::net
{

/// Asynchronous HTTP/1.1 GET-style request used by the OSCQuery mirror and
/// the QML HTTP protocol.
///
/// This is a thin wrapper around boost::beast::http::async_* that preserves
/// the historical Fun/Err callable interface used by callers:
///   - Fun::reserve_expect: static constexpr buffer size hint (no longer
///     load-bearing — Beast manages its own buffer — but kept for ABI
///     compatibility)
///   - Fun(*this, body): success callback, body is passed as std::string
///   - Err(*this): error callback
///
/// Typical use:
///   auto req = std::make_shared<http_get_request<F, E>>(
///       F{}, E{}, ctx, "example.com", "/some/path");
///   req->resolve("example.com", "80");
template <typename Fun, typename Err>
class http_get_request
    : public std::enable_shared_from_this<http_get_request<Fun, Err>>
{
public:
  using std::enable_shared_from_this<http_get_request<Fun, Err>>::shared_from_this;

  http_get_request(
      Fun f, Err err, boost::asio::io_context& ctx, const std::string& server,
      const std::string& path, std::string_view verb = "GET")
      : m_resolver{boost::asio::make_strand(ctx)}
      , m_stream{boost::asio::make_strand(ctx)}
      , m_host{server}
      , m_fun{std::move(f)}
      , m_err{std::move(err)}
  {
    namespace http = boost::beast::http;

    auto v = http::string_to_verb(std::string{verb});
    if(v == http::verb::unknown)
      v = http::verb::get;
    m_req.method(v);
    m_req.version(11);
    m_req.target(percent_encode_spaces(path));
    m_req.set(http::field::host, server);
    m_req.set(http::field::accept, "*/*");
    m_req.set(http::field::user_agent, "ossia");
    m_req.set(http::field::connection, "close");
  }

  void resolve(const std::string& server, const std::string& port)
  {
    m_resolver.async_resolve(
        server, port,
        [self = this->shared_from_this()](
            const boost::beast::error_code& ec,
            const boost::asio::ip::tcp::resolver::results_type& results) {
      self->on_resolve(ec, results);
    });
  }

  void close()
  {
    boost::beast::error_code ec;
    m_stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    m_stream.socket().close(ec);
  }

private:
  static std::string percent_encode_spaces(const std::string& path)
  {
    // Technically other characters should be encoded too, but they aren't
    // legal in OSC address patterns — preserve historical behaviour.
    std::string out;
    out.reserve(path.size());
    for(char c : path)
    {
      if(c == ' ')
        out += "%20";
      else
        out.push_back(c);
    }
    return out;
  }

  void on_resolve(
      const boost::beast::error_code& ec,
      const boost::asio::ip::tcp::resolver::results_type& results)
  {
    if(ec)
    {
      ossia::logger().error("HTTP Error: {}", ec.message());
      m_err(*this);
      return;
    }
    m_stream.expires_after(std::chrono::seconds(30));
    m_stream.async_connect(
        results,
        [self = this->shared_from_this()](
            const boost::beast::error_code& ec,
            const boost::asio::ip::tcp::resolver::results_type::endpoint_type&) {
      self->on_connect(ec);
    });
  }

  void on_connect(const boost::beast::error_code& ec)
  {
    if(ec)
    {
      ossia::logger().error("HTTP Error: {}", ec.message());
      m_err(*this);
      return;
    }
    m_stream.expires_after(std::chrono::seconds(30));
    boost::beast::http::async_write(
        m_stream, m_req,
        [self = this->shared_from_this()](
            const boost::beast::error_code& ec, std::size_t bytes) {
      self->on_write(ec, bytes);
    });
  }

  void on_write(const boost::beast::error_code& ec, std::size_t)
  {
    if(ec)
    {
      ossia::logger().error("HTTP Error: {}", ec.message());
      m_err(*this);
      return;
    }
    m_stream.expires_after(std::chrono::seconds(30));
    // Use an explicit parser with a generous body limit: OSCQuery namespace
    // dumps regularly exceed Beast's default 1 MiB cap.
    m_parser.body_limit(256u * 1024u * 1024u);
    boost::beast::http::async_read(
        m_stream, m_buffer, m_parser,
        [self = this->shared_from_this()](
            const boost::beast::error_code& ec, std::size_t bytes) {
      self->on_read(ec, bytes);
    });
  }

  void on_read(const boost::beast::error_code& ec, std::size_t)
  {
    if(ec && ec != boost::beast::http::error::end_of_stream
       && ec != boost::asio::error::eof)
    {
      ossia::logger().error("HTTP Error: {}", ec.message());
      m_err(*this);
      return;
    }

    auto& res = m_parser.get();
    auto status = res.result_int();
    if(status != 200)
    {
      ossia::logger().error("HTTP Error: status code {}", (int)status);
      m_err(*this);
      close();
      return;
    }

    // Reserve a small tail so RapidJSON's SIMD parser, which reads past
    // the end of the buffer, doesn't overrun. The previous implementation
    // baked the same +16 reservation in.
    auto& body = res.body();
    std::string out;
    out.reserve(body.size() + 16);
    out.assign(body.begin(), body.end());

    m_fun(*this, out);
    close();
  }

  boost::asio::ip::tcp::resolver m_resolver;
  boost::beast::tcp_stream m_stream;
  boost::beast::flat_buffer m_buffer;
  boost::beast::http::request<boost::beast::http::empty_body> m_req;
  boost::beast::http::response_parser<boost::beast::http::string_body> m_parser;
  std::string m_host;
  Fun m_fun;
  Err m_err;
};

}
