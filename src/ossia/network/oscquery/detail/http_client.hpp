#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/logger.hpp>

#include <asio.hpp>
#include <asio/placeholders.hpp>

#include <utility>

namespace ossia
{
namespace oscquery
{
using tcp = asio::ip::tcp;

template <typename Fun, typename Err>
class http_get_request
    : public std::enable_shared_from_this<http_get_request<Fun, Err>>
{
  fmt::memory_buffer m_request;

public:
  using std::enable_shared_from_this<
      http_get_request<Fun, Err>>::shared_from_this;
  http_get_request(
      Fun f, Err err, asio::io_context& ctx, const std::string& server,
      const std::string& path)
      : m_resolver(ctx)
      , m_socket(ctx)
      , m_fun{std::move(f)}
      , m_err{std::move(err)}
  {
    m_request.reserve(100 + server.size() + path.size());
    fmt::format_to(m_request, "GET ");
    // Technically other characters should be encoded... but
    // they aren't legal in OSC address patterns.
    for (auto c : path)
      if (c != ' ')
        fmt::format_to(m_request, "{}", c);
      else
        fmt::format_to(m_request, "%20");


    fmt::format_to(m_request,
                   " HTTP/1.1\r\n"
                   "Host: {}"
                   "\r\n"
                   "Accept: */*\r\n"
                   "Connection: close\r\n\r\n", server);
  }

  void resolve(const std::string& server, const std::string& port)
  {
    m_resolver.async_resolve(
        server, port,
        [self = this->shared_from_this()](
            const asio::error_code& err,
            const tcp::resolver::results_type& endpoints) {
          self->handle_resolve(err, endpoints);
        });
  }

  void close()
  {
    m_socket.close();
  }

private:
  void handle_resolve(
      const asio::error_code& err,
      const tcp::resolver::results_type& endpoints)
  {
    if (!err)
    {
      asio::async_connect(
          m_socket, endpoints,
          [self = shared_from_this()](const asio::error_code& err, auto&&) {
            self->handle_connect(err);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_connect(const asio::error_code& err)
  {
    if (!err)
    {
      asio::const_buffer request(m_request.data(), m_request.size());
      asio::async_write(
          m_socket, request,
          [self = shared_from_this()](const asio::error_code& err, auto&&...) {
            self->handle_write_request(err);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_write_request(const asio::error_code& err)
  {
    if (!err)
    {
      asio::async_read_until(
          m_socket, m_response, "\r\n",
          [self = shared_from_this()](const asio::error_code& err, auto&&...) {
            self->handle_read_status_line(err);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_read_status_line(const asio::error_code& err)
  {
    if (!err)
    {
      // Check that response is OK.
      std::istream response_stream(&m_response);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/")
      {
        ossia::logger().error("HTTP Error: Invalid response");
        return;
      }
      if (status_code != 200)
      {
        ossia::logger().error("HTTP Error: status code {}", status_code);
        return;
      }

      // Read the response headers, which are terminated by a blank line.
      asio::async_read_until(
          m_socket, m_response, "\r\n\r\n",
          [self = shared_from_this()](const asio::error_code& err, auto&&...) {
            self->handle_read_headers(err);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_read_headers(const asio::error_code& err)
  {
    if (!err)
    {
      // Process the response headers.
      std::istream response_stream(&m_response);
      std::string header;
      while (std::getline(response_stream, header) && header != "\r")
        ;

      // Start reading remaining data until EOF.
      asio::async_read(
          m_socket, m_response, asio::transfer_at_least(1),
          [self = shared_from_this()](const asio::error_code& err, auto&&...) {
            self->handle_read_content(err);
          });
    }
    else
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
  }

  void handle_read_content(const asio::error_code& err)
  {
    if (!err)
    {
      // Continue reading remaining data until EOF.
      asio::async_read(
          m_socket, m_response, asio::transfer_at_least(1),
          [self = shared_from_this()](const asio::error_code& err, auto&&...) {
            self->handle_read_content(err);
          });
    }
    else if (err != asio::error::eof)
    {
      ossia::logger().error("HTTP Error: {}", err.message());
      m_err(*this);
    }
    else if (err == asio::error::eof)
    {
      // Write all of the data that has been read so far.
      const auto& dat = m_response.data();
      auto begin = asio::buffers_begin(dat);
      auto end = asio::buffers_end(dat);
      auto sz = end - begin;
      std::string str;
      str.reserve(
          sz + 16); // for RapidJSON simd parsing which reads past bounds
      str.assign(begin, end);
      m_fun(*this, str);
    }
  }

  tcp::resolver m_resolver;
  tcp::socket m_socket;
  asio::streambuf m_response;
  Fun m_fun;
  Err m_err;
};
}
}
