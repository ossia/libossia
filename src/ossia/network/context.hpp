#pragma once
#include <ossia/detail/logger.hpp>

#include <boost/asio/io_context.hpp>
#include <memory>

namespace ossia::net
{
struct network_context
{
  boost::asio::io_context context;

  void run()
  {
    using work_guard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

    work_guard wg{context.get_executor()};
    try {
      context.run();
    } catch (std::exception& e) {
      ossia::logger().error("Error while processing network events: {}", e.what());
    } catch (...) {
      ossia::logger().error("Error while processing network events.");
    }
  }
};
using network_context_ptr = std::shared_ptr<network_context>;
}
