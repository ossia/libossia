#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/fmt.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/parse_relax.hpp>

#include <boost/asio.hpp>

#include <utility>
#include <vector>

namespace ossia::net
{
using tcp = boost::asio::ip::tcp;

// Verbs are case-sensitive on the wire, but a script may well send "head":
// comparing ASCII-case-insensitively keeps a bodiless reply from being taken
// for a truncated one. `upper` is expected to be made of letters only.
inline bool http_verb_is(std::string_view verb, std::string_view upper) noexcept
{
  if(verb.size() != upper.size())
    return false;
  for(std::size_t i = 0; i < verb.size(); ++i)
    if((verb[i] | 0x20) != (upper[i] | 0x20))
      return false;
  return true;
}

// Full HTTP client supporting all methods, custom headers, request body.
// Success callback receives (request, status_code, response_body).
// Error callback receives (request, error_message).
template <typename Fun, typename Err>
class http_client_request
    : public std::enable_shared_from_this<http_client_request<Fun, Err>>
{
  fmt::memory_buffer m_request;

public:
  using std::enable_shared_from_this<http_client_request<Fun, Err>>::shared_from_this;

  http_client_request(
      Fun f, Err err, boost::asio::io_context& ctx, std::string_view verb,
      std::string_view host, std::string_view path,
      const std::vector<std::pair<std::string, std::string>>& headers = {},
      std::string_view body = {})
      : m_resolver(ctx)
      , m_socket(ctx)
      , m_headRequest{http_verb_is(verb, "HEAD")}
      , m_connectRequest{http_verb_is(verb, "CONNECT")}
      , m_fun{std::move(f)}
      , m_err{std::move(err)}
  {
    m_request.reserve(256 + host.size() + path.size() + body.size());
    m_response.prepare(Fun::reserve_expect);

    // Request line: VERB /path HTTP/1.1
    fmt::format_to(fmt::appender(m_request), "{} ", verb);
    for(auto c : path)
    {
      if(c != ' ')
        fmt::format_to(fmt::appender(m_request), "{}", c);
      else
        fmt::format_to(fmt::appender(m_request), "%20");
    }
    fmt::format_to(fmt::appender(m_request), " HTTP/1.1\r\n");

    // Host header (always required)
    fmt::format_to(fmt::appender(m_request), "Host: {}\r\n", host);

    // Track which default headers the user already provided
    bool hasAccept = false;
    bool hasConnection = false;
    bool hasContentLength = false;
    bool hasContentType = false;

    // User-supplied headers
    for(const auto& [key, value] : headers)
    {
      fmt::format_to(fmt::appender(m_request), "{}: {}\r\n", key, value);
      if(key == "Accept")
        hasAccept = true;
      else if(key == "Connection")
        hasConnection = true;
      else if(key == "Content-Length")
        hasContentLength = true;
      else if(key == "Content-Type")
        hasContentType = true;
    }

    // Fill in defaults for headers the user didn't set
    if(!hasAccept)
      fmt::format_to(fmt::appender(m_request), "Accept: */*\r\n");
    if(!hasConnection)
      fmt::format_to(fmt::appender(m_request), "Connection: close\r\n");

    if(!body.empty())
    {
      if(!hasContentLength)
        fmt::format_to(
            fmt::appender(m_request), "Content-Length: {}\r\n", body.size());
      if(!hasContentType)
        fmt::format_to(
            fmt::appender(m_request), "Content-Type: application/octet-stream\r\n");
    }

    // End of headers + body
    fmt::format_to(fmt::appender(m_request), "\r\n");
    if(!body.empty())
      fmt::format_to(fmt::appender(m_request), "{}", body);
  }

  void resolve(const std::string& server, const std::string& port)
  {
    m_resolver.async_resolve(
        server, port,
        [self = this->shared_from_this()](
            const boost::system::error_code& err,
            const tcp::resolver::results_type& endpoints) {
      self->handle_resolve(err, endpoints);
    });
  }

  void close() { m_socket.close(); }

private:
  void handle_resolve(
      const boost::system::error_code& err,
      const tcp::resolver::results_type& endpoints)
  {
    if(!err)
    {
      boost::asio::async_connect(
          m_socket, endpoints,
          [self = this->shared_from_this()](
              const boost::system::error_code& err, auto&&...) {
        self->handle_connect(err);
      });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this, err.message());
    }
  }

  void handle_connect(const boost::system::error_code& err)
  {
    if(!err)
    {
      boost::asio::const_buffer request(m_request.data(), m_request.size());
      boost::asio::async_write(
          m_socket, request,
          [self = this->shared_from_this()](
              const boost::system::error_code& err, std::size_t size) {
        self->handle_write_request(err, size);
      });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this, err.message());
    }
  }

  void handle_write_request(const boost::system::error_code& err, std::size_t size)
  {
    if(!err)
    {
      boost::asio::async_read_until(
          m_socket, m_response, "\r\n",
          [self = this->shared_from_this()](
              const boost::system::error_code& err, std::size_t size) {
        self->handle_read_status_line(err, size);
      });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this, err.message());
    }
  }

  void handle_read_status_line(const boost::system::error_code& err, std::size_t size)
  {
    if(!err || err == boost::asio::error::eof)
    {
      std::istream response_stream(&m_response);
      std::string http_version;
      response_stream >> http_version;
      response_stream >> m_statusCode;
      std::string status_message;
      std::getline(response_stream, status_message);

      if(!response_stream || http_version.substr(0, 5) != "HTTP/")
      {
        ossia::logger().error("HTTP Error: Invalid response");
        m_err(*this, "Invalid HTTP response");
        return;
      }

      read_header_line();
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this, err.message());
    }
  }

  // Headers are read one field at a time: the status line's CRLF has already
  // been consumed, so a response carrying no header field at all has only the
  // empty line left and waiting for "\r\n\r\n" would never complete.
  void read_header_line()
  {
    boost::asio::async_read_until(
        m_socket, m_response, "\r\n",
        [self = this->shared_from_this()](
            const boost::system::error_code& err, std::size_t size) {
      self->handle_read_header_line(err, size);
    });
  }

  void handle_read_header_line(const boost::system::error_code& err, std::size_t size)
  {
    if(err && err != boost::asio::error::eof)
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this, err.message());
      return;
    }

    std::istream response_stream(&m_response);
    std::string header;
    if(!std::getline(response_stream, header))
    {
      ossia::logger().error("HTTP Error: response ended inside its headers");
      m_err(*this, "HTTP response ended inside its headers");
      return;
    }

    if(!(header.empty() || header == "\r"))
    {
      if(header.starts_with("Content-Length: "))
      {
        std::string_view sz(header.begin() + strlen("Content-Length: "), header.end());
        if(auto num = ossia::parse_relax<int>(sz))
          m_contentLength = *num;
      }
      read_header_line();
      return;
    }

    handle_headers_complete(size);
  }

  void handle_headers_complete(std::size_t size)
  {
    // 1xx are interim: the real response follows on the same connection
    // (RFC 9110 §15.2), and whatever is already buffered belongs to it.
    if(m_statusCode >= 100 && m_statusCode < 200)
    {
      m_statusCode = 0;
      m_contentLength = -1;
      boost::asio::async_read_until(
          m_socket, m_response, "\r\n",
          [self = this->shared_from_this()](
              const boost::system::error_code& err, std::size_t size) {
        self->handle_read_status_line(err, size);
      });
      return;
    }

    // HEAD replies, 204, 304 and a successful CONNECT never carry a body: their
    // Content-Length, if any, describes what a GET would have returned.
    const bool bodiless
        = m_headRequest || m_statusCode == 204 || m_statusCode == 304
          || (m_connectRequest && m_statusCode >= 200 && m_statusCode < 300);
    if(bodiless)
    {
      // Anything that follows belongs to no response of ours
      m_response.consume(m_response.size());
      m_contentLength = 0;
      finish_read(boost::asio::error::eof, 0);
    }
    else if(m_contentLength == 0)
    {
      finish_read(boost::asio::error::eof, 0);
    }
    else if(m_contentLength > 0)
    {
      if(m_contentLength <= (int)m_response.size())
      {
        finish_read(boost::asio::error::eof, size);
      }
      else
      {
        boost::asio::async_read(
            m_socket, m_response,
            boost::asio::transfer_exactly(m_contentLength - m_response.size()),
            [self = this->shared_from_this()](
                const boost::system::error_code& err, std::size_t size) {
          self->handle_read_content(err, size);
        });
      }
    }
    else
    {
      // No Content-Length — read until EOF
      boost::asio::async_read(
          m_socket, m_response, boost::asio::transfer_all(),
          [self = this->shared_from_this()](
              const boost::system::error_code& err, std::size_t size) {
        self->handle_read_content(err, size);
      });
    }
  }

  void handle_read_content(const boost::system::error_code& err, std::size_t size)
  {
    if(!err)
    {
      finish_read(err, size);
    }
    else if(err == boost::asio::error::eof)
    {
      // EOF only delimits the body when no Content-Length was advertised:
      // otherwise the peer closed before sending the announced payload.
      if(m_contentLength > 0 && (int)m_response.size() < m_contentLength)
      {
        ossia::logger().error("HTTP Error: response truncated before Content-Length");
        m_err(*this, "HTTP response truncated before Content-Length");
        return;
      }
      finish_read(err, size);
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this, err.message());
    }
  }

  void finish_read(const boost::system::error_code& err, std::size_t size)
  {
    const auto& dat = m_response.data();
    auto begin = boost::asio::buffers_begin(dat);
    auto end = boost::asio::buffers_end(dat);
    auto sz = end - begin;

    // Content-Length is an upper bound as well: bytes read past it belong to
    // whatever follows on the connection, never to this body.
    if(m_contentLength >= 0 && sz > (std::ptrdiff_t)m_contentLength)
    {
      sz = m_contentLength;
      end = begin + sz;
    }

    std::string str;
    str.reserve(sz + 16);
    str.assign(begin, end);
    m_fun(*this, m_statusCode, str);
    close();
  }

  tcp::resolver m_resolver;
  tcp::socket m_socket;
  boost::asio::streambuf m_response;
  int m_contentLength{-1};
  bool m_headRequest{};
  bool m_connectRequest{};
  int m_statusCode{0};
  Fun m_fun;
  Err m_err;
};
}
