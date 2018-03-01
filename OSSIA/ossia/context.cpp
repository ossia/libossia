// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "context.hpp"
#include <ossia/detail/logger.hpp>
#include <spdlog/spdlog.h>
#if defined(QT_QML_LIB)
#include <ossia-qt/qml_plugin.hpp>
#endif
#include <smallfun.hpp>
#include <iostream>
#include <memory>

#include <ossia/detail/callback_container.hpp>

namespace ossia
{
invalid_callback_error::~invalid_callback_error()
{

}

static void ossia_global_init()
{
  static bool init = false;
  if (!init)
  {

    // Create a logger for the library.
    logger();

// Register QML types
#if defined(QT_QML_LIB)
    qt::qml_plugin::reg("Ossia");
#endif
  }
}

context::context()
{
  ossia_global_init();
}

context::context(const std::vector<spdlog::sink_ptr>& sinks)
{
  spdlog::create("ossia", sinks.begin(), sinks.end());
  ossia_global_init();
}

context::~context() = default;

spdlog::logger& logger()
{
  static spdlog::logger& init = []() -> spdlog::logger& {
    return *logger_ptr();
  }();

  return init;
}

std::shared_ptr<spdlog::logger> logger_ptr()
{
  if (auto logger = spdlog::get("ossia"))
    return logger;
  else
    return spdlog::stderr_logger_mt("ossia");
}
}
