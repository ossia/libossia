#pragma once
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
