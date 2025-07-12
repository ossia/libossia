#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/fmt.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/detail/parse_relax.hpp>

#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>

#include <utility>

namespace ossia::net
{
using tcp = boost::asio::ip::tcp;

template <typename Fun, typename Err>
class http_get_request : public std::enable_shared_from_this<http_get_request<Fun, Err>>
{
  fmt::memory_buffer m_request;

public:
  using std::enable_shared_from_this<http_get_request<Fun, Err>>::shared_from_this;
  http_get_request(
      Fun f, Err err, boost::asio::io_context& ctx, const std::string& server,
      const std::string& path, std::string_view verb = "GET")
      : m_resolver(ctx)
      , m_socket(ctx)
      , m_fun{std::move(f)}
      , m_err{std::move(err)}
  {
    m_request.reserve(100 + server.size() + path.size());
    m_response.prepare(Fun::reserve_expect);
    fmt::format_to(fmt::appender(m_request), "{} ", verb);
    // Technically other characters should be encoded... but
    // they aren't legal in OSC address patterns.
    for(auto c : path)
      if(c != ' ')
        fmt::format_to(fmt::appender(m_request), "{}", c);
      else
        fmt::format_to(fmt::appender(m_request), "%20");

    fmt::format_to(
        fmt::appender(m_request),
        " HTTP/1.1\r\n"
        "Host: {}\r\n"
        "Accept: */*\r\n"
        "Connection: close\r\n\r\n",
        server);
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
      const boost::system::error_code& err, const tcp::resolver::results_type& endpoints)
  {
    if(!err)
    {
      boost::asio::async_connect(
          m_socket, endpoints,
          [self = shared_from_this()](const boost::system::error_code& err, auto&&...) {
        self->handle_connect(err);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_connect(const boost::system::error_code& err)
  {
    if(!err)
    {
      boost::asio::const_buffer request(m_request.data(), m_request.size());
      boost::asio::async_write(
          m_socket, request,
          [self = shared_from_this()](
              const boost::system::error_code& err, std::size_t size) {
        self->handle_write_request(err, size);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_write_request(const boost::system::error_code& err, std::size_t size)
  {
    if(!err)
    {
      boost::asio::async_read_until(
          m_socket, m_response, "\r\n",
          [self = shared_from_this()](
              const boost::system::error_code& err, std::size_t size) {
        self->handle_read_status_line(err, size);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_read_status_line(const boost::system::error_code& err, std::size_t size)
  {
    if(!err)
    {
      // Check that response is OK.
      std::istream response_stream(&m_response);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if(!response_stream || http_version.substr(0, 5) != "HTTP/")
      {
        ossia::logger().error("HTTP Error: Invalid response");
        return;
      }
      if(status_code != 200)
      {
        ossia::logger().error("HTTP Error: status code {}", status_code);
        return;
      }

      // Read the response headers, which are terminated by a blank line.
      boost::asio::async_read_until(
          m_socket, m_response, "\r\n\r\n",
          [self = shared_from_this()](
              const boost::system::error_code& err, std::size_t size) {
        self->handle_read_headers(err, size);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_read_headers(const boost::system::error_code& err, std::size_t size)
  {
    if(!err)
    {
      // Process the response headers.
      std::istream response_stream(&m_response);
      std::string header;
      while(std::getline(response_stream, header) && header != "\r")
      {
        if(header.starts_with("Content-Length: "))
        {
          std::string_view sz(header.begin() + strlen("Content-Length: "), header.end());
          if(auto num = ossia::parse_relax<int>(sz))
          {
            m_contentLength = *num;
          }
          else
          {
            ossia::logger().error("Invalid HTTP Content-length: {}", sz);
            return;
          }
        }
      }

      if(m_contentLength > 0)
      {
        if(m_contentLength == m_response.size())
        {
          finish_read(boost::asio::error::eof, size);
        }
        else
        {
          boost::asio::async_read(
              m_socket, m_response, boost::asio::transfer_exactly(m_contentLength - m_response.size()),
              [self = shared_from_this()](
                  const boost::system::error_code& err, std::size_t size) {
            self->handle_read_content(err, size);
          });
        }
      }
      else
      {
        // Start reading remaining data until EOF.
        boost::asio::async_read(
            m_socket, m_response, boost::asio::transfer_all(),
            [self = shared_from_this()](
                const boost::system::error_code& err, std::size_t size) {
          self->handle_read_content(err, size);
        });
      }
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_read_content(const boost::system::error_code& err, std::size_t size)
  {
    if(!err || err == boost::asio::error::eof)
      finish_read(err, size);
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void finish_read(const boost::system::error_code& err, std::size_t size)
  {
    const auto& dat = m_response.data();
    auto begin = boost::asio::buffers_begin(dat);
    auto end = boost::asio::buffers_end(dat);
    auto sz = end - begin;
    std::string str;
    str.reserve(sz + 16); // for RapidJSON simd parsing which reads past bounds
    str.assign(begin, end);
    m_fun(*this, str);
    close();
  }

  tcp::resolver m_resolver;
  tcp::socket m_socket;
  boost::asio::streambuf m_response;
  int m_contentLength{-1};
  Fun m_fun;
  Err m_err;
};
}
