#pragma once
#include <ossia/network/context.hpp>
#include <ossia/network/http/http_client.hpp>

namespace ossia::oscquery_asio
{

template <typename State>
struct http_async_answer
{
  static constexpr auto reserve_expect = 65535 * 8;
  std::weak_ptr<State> state;

  template <typename T, typename S>
  void operator()(T& req, const S& str)
  {
    if(auto ptr = state.lock())
    {
      if(ptr->active)
      {
        ptr->self.on_text_ws_message({}, str);
      }
    }
  }
};

template <typename State>
struct http_async_value_answer
{
  static constexpr auto reserve_expect = 1500;
  std::weak_ptr<State> state;
  std::string source_address;

  template <typename T, typename S>
  void operator()(T& req, const S& str)
  {
    if(auto ptr = state.lock())
    {
      if(ptr->active)
      {
        ptr->self.on_value_http_message(source_address, str);
      }
    }
  }
};

struct http_async_error
{
  template <typename T>
  void operator()(T& req)
  {
    req.close();
  }
};

template <typename T>
auto wait_for_future(
    const std::future<T>& fut, std::chrono::milliseconds timeout,
    boost::asio::io_context& ctx)
{
  std::future_status status = fut.wait_for(std::chrono::seconds(0));

  auto t0 = std::chrono::steady_clock::now();

  while(status != std::future_status::ready)
  {
    ctx.poll_one();
    if((status = fut.wait_for(std::chrono::seconds(0))) == std::future_status::ready)
      return status;

    auto t1 = std::chrono::steady_clock::now();
    if(std::chrono::duration_cast<std::chrono::seconds>(t1 - t0) > timeout)
    {
      return status;
      break;
    }
  }

  return status;
}
template <typename State>
struct http_async_request
    : ossia::net::http_get_request<http_async_answer<State>, http_async_error>
{
  using ossia::net::http_get_request<
      http_async_answer<State>, http_async_error>::http_get_request;
};

template <typename State>
struct http_async_value_request
    : ossia::net::http_get_request<http_async_value_answer<State>, http_async_error>
{
  using ossia::net::http_get_request<
      http_async_value_answer<State>, http_async_error>::http_get_request;
};

struct http_async_client_context
{
  std::shared_ptr<boost::asio::io_service::work> worker;
};

}
