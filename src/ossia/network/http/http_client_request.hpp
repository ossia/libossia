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

      // Read headers (terminated by blank line)
      boost::asio::async_read_until(
          m_socket, m_response, "\r\n\r\n",
          [self = this->shared_from_this()](
              const boost::system::error_code& err, std::size_t size) {
        self->handle_read_headers(err, size);
      });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this, err.message());
    }
  }

  void handle_read_headers(const boost::system::error_code& err, std::size_t size)
  {
    if(!err || err == boost::asio::error::eof)
    {
      std::istream response_stream(&m_response);
      std::string header;
      while(std::getline(response_stream, header) && header != "\r")
      {
        if(header.starts_with("Content-Length: "))
        {
          std::string_view sz(
              header.begin() + strlen("Content-Length: "), header.end());
          if(auto num = ossia::parse_relax<int>(sz))
            m_contentLength = *num;
        }
      }

      if(m_contentLength == 0)
      {
        // Empty body (e.g. 204 No Content)
        finish_read(boost::asio::error::eof, 0);
      }
      else if(m_contentLength > 0)
      {
        if(m_contentLength == (int)m_response.size())
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
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this, err.message());
    }
  }

  void handle_read_content(const boost::system::error_code& err, std::size_t size)
  {
    if(!err || err == boost::asio::error::eof)
      finish_read(err, size);
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
  int m_statusCode{0};
  Fun m_fun;
  Err m_err;
};
}
