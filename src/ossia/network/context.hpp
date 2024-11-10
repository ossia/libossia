#pragma once

#if defined(_WIN32)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
#define NOMINMAX
#endif
#if !defined(UNICODE)
#define UNICODE 1
#endif
#if !defined(_UNICODE)
#define _UNICODE 1
#endif
// clang-format off
#include <winsock2.h>
#include <windows.h>
// clang-format on
#endif

#include <ossia/detail/logger.hpp>

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

#include <memory>

namespace ossia::net
{
using strand_type
    = decltype(boost::asio::make_strand(std::declval<boost::asio::io_context&>()));
struct network_context
{
  boost::asio::io_context context;

  void run()
  {
    auto wg = boost::asio::make_work_guard(context);
#if defined(__cpp_exceptions)
    try
    {
      context.run();
    }
    catch(std::exception& e)
    {
      ossia::logger().error("Error while processing network events: {}", e.what());
    }
    catch(...)
    {
      ossia::logger().error("Error while processing network events.");
    }
#else
    context.run();
#endif
    context.restart();
  }
};
using network_context_ptr = std::shared_ptr<network_context>;
}
