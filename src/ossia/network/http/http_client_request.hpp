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
#include <vector>

namespace ossia::net
{

/// Full HTTP/1.1 client supporting all methods, custom headers and a
/// request body. Used by the QML protocols layer.
///
/// Compared to ossia::net::http_get_request, the success callback receives
/// the HTTP status code and the body:
///   Fun(*this, status_code, body)
///   Err(*this, error_message)
template <typename Fun, typename Err>
class http_client_request
    : public std::enable_shared_from_this<http_client_request<Fun, Err>>
{
public:
  using std::enable_shared_from_this<http_client_request<Fun, Err>>::shared_from_this;

  http_client_request(
      Fun f, Err err, boost::asio::io_context& ctx, std::string_view verb,
      std::string_view host, std::string_view path,
      const std::vector<std::pair<std::string, std::string>>& headers = {},
      std::string_view body = {})
      : m_resolver{boost::asio::make_strand(ctx)}
      , m_stream{boost::asio::make_strand(ctx)}
      , m_host{host}
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
    m_req.set(http::field::host, std::string{host});

    // Track which default headers the user already provided so we don't
    // override them.
    bool has_accept = false;
    bool has_connection = false;
    bool has_content_type = false;

    for(const auto& [key, value] : headers)
    {
      m_req.set(key, value);
      if(key == "Accept")
        has_accept = true;
      else if(key == "Connection")
        has_connection = true;
      else if(key == "Content-Type")
        has_content_type = true;
    }

    if(!has_accept)
      m_req.set(http::field::accept, "*/*");
    if(!has_connection)
      m_req.set(http::field::connection, "close");

    if(!body.empty())
    {
      if(!has_content_type)
        m_req.set(http::field::content_type, "application/octet-stream");
      m_req.body().assign(body.data(), body.size());
      m_req.prepare_payload();
    }
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
  static std::string percent_encode_spaces(std::string_view path)
  {
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
      m_err(*this, ec.message());
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
      m_err(*this, ec.message());
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
      m_err(*this, ec.message());
      return;
    }
    m_stream.expires_after(std::chrono::seconds(30));
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
      m_err(*this, ec.message());
      return;
    }

    auto& res = m_parser.get();
    int status = (int)res.result_int();
    const auto& body = res.body();
    m_fun(*this, status, std::string_view{body.data(), body.size()});
    close();
  }

  boost::asio::ip::tcp::resolver m_resolver;
  boost::beast::tcp_stream m_stream;
  boost::beast::flat_buffer m_buffer;
  boost::beast::http::request<boost::beast::http::string_body> m_req;
  boost::beast::http::response_parser<boost::beast::http::string_body> m_parser;
  std::string m_host;
  Fun m_fun;
  Err m_err;
};

}
