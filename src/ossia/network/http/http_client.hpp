#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/network/http/http_client_request.hpp>

#include <memory>
#include <string>
#include <utility>

namespace ossia::net
{
//! GET-oriented facade over http_client_request.
//!
//! It only adapts the callback shapes of the OSCQuery mirrors - a body-only
//! answer and an argument-less error - onto the client implementation. No
//! protocol handling here: response framing lives in http_client_request.
template <typename Fun, typename Err>
class http_get_request
{
  struct answer
  {
    static constexpr int reserve_expect = Fun::reserve_expect;
    Fun fun;
    Err err;

    void operator()(auto& req, int status, const auto& body)
    {
      // The legacy callback carries no status, so only a successful response
      // can be handed to it as a body.
      if(status >= 200 && status < 300)
      {
        fun(req, body);
      }
      else
      {
        ossia::logger().error("HTTP Error: status code {}", status);
        err(req);
      }
    }
  };

  struct error
  {
    Err err;
    void operator()(auto& req, std::string_view) { err(req); }
  };

  using implementation = http_client_request<answer, error>;

public:
  http_get_request(
      Fun f, Err err, boost::asio::io_context& ctx, const std::string& server,
      const std::string& path, std::string_view verb = "GET")
      : m_impl{std::make_shared<implementation>(
            answer{std::move(f), err}, error{err}, ctx, verb, server, path)}
  {
  }

  //! The request keeps itself alive until it completes, like the
  //! implementation it delegates to.
  void resolve(const std::string& server, const std::string& port)
  {
    m_impl->resolve(server, port);
  }

  void close() { m_impl->close(); }

private:
  std::shared_ptr<implementation> m_impl;
};
}
