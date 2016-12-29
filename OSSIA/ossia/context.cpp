#include "context.hpp"
#if defined(QT_QML_LIB)
#include <qqml.h>
#include <ossia/network/common/js_utilities.hpp>
#endif

namespace ossia
{
context::context()
{
  // Create a logger for the library.
  spdlog::stderr_logger_mt("ossia");
#if defined(QT_QML_LIB)
  qmlRegisterUncreatableType<ossia::net::qml_context>("org.ossia", 1, 0, "Ossia", "");
#endif
}

context::~context() = default;

spdlog::logger& logger()
{
  static auto& init = [] () -> spdlog::logger& {
    if(auto logger = spdlog::get("ossia"))
      return *logger;

    return *spdlog::stderr_logger_mt("ossia");
  }();

  return init;
}
}
