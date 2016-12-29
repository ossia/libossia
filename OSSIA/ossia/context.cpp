#include "context.hpp"
#if defined(QT_QML_LIB)
#include <qqml.h>
#include <ossia/network/common/js_utilities.hpp>
#endif

namespace ossia
{
static void ossia_global_init()
{
  // Create a logger for the library.
  logger();

#if defined(QT_QML_LIB)
  qmlRegisterUncreatableType<ossia::net::qml_context>("org.ossia", 1, 0, "Ossia", "");
#endif
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
  static spdlog::logger& init = [] () -> spdlog::logger& {
      auto logger = spdlog::get("ossia");
      if(logger)
        return *logger;
      else
        return *spdlog::stderr_logger_mt("ossia");
}();

  return init;
}
}
