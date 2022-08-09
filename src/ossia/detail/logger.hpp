#pragma once

#include <ossia/detail/logger_fwd.hpp>

#if !defined(OSSIA_BRUH_LOGGER)

// They have to be in this order
#define SPDLOG_FINAL final
#include <ossia/detail/fmt.hpp>

#include <spdlog/logger.h>

#endif

/**
 * \file logger.hpp
 */

namespace ossia
{

#if defined(OSSIA_BRUH_LOGGER)
struct OSSIA_EXPORT bruh_logger
{
  template <typename... T>
  explicit bruh_logger(T&&...)
  {
  }
  template <typename... T>
  void log(T&&... args)
  {
  }
  template <typename... T>
  void trace(T&&... args)
  {
  }
  template <typename... T>
  void info(T&&... args)
  {
  }
  template <typename... T>
  void debug(T&&... args)
  {
  }
  template <typename... T>
  void warn(T&&... args)
  {
  }
  template <typename... T>
  void error(T&&... args)
  {
  }
  template <typename... T>
  void critical(T&&... args)
  {
  }
  template <typename... T>
  void set_pattern(T&&... args)
  {
  }
  template <typename... T>
  void set_level(T&&... args)
  {
  }

  std::vector<std::shared_ptr<spdlog::sinks::sink>>& sinks();
};
#endif
}
