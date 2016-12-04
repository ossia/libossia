#pragma once
#include <memory>
#include <functional>
#include <ossia_export.h>
#include <string>

namespace spdlog
{
// Use it by doing : #include <ossia/detail/logger.hpp>
class logger;
}

namespace ossia
{
namespace net
{
class OSSIA_EXPORT network_logger
{
public:
  network_logger();
  ~network_logger();

  /**
   * @brief inbound_logger Set log function for messages coming from
   * outside.
   */
  std::shared_ptr<spdlog::logger> inbound_logger;

  /**
   * @brief outbound_logger Set log function for messages going outside.
   */
  std::shared_ptr<spdlog::logger> outbound_logger;

  /**
   * @brief outbound_logger Set log function for messages going outside.
   */
  std::shared_ptr<spdlog::logger> error_logger;

};

}
}
