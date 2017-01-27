#include "context.hpp"
#if defined(QT_QML_LIB)
#include <qqml.h>
#include <ossia-qt/js_utilities.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/device/qml_impulse.hpp>
#endif

namespace ossia
{
static void ossia_global_init()
{
  // Create a logger for the library.
  logger();

#if defined(QT_QML_LIB)
    qmlRegisterUncreatableType<qt::qml_context>("org.ossia", 1, 0, "Ossia", "");

    qmlRegisterSingletonType<qt::qml_singleton_device>("org.ossia", 1, 0, "OssiaSingleDevice",
                             [] (QQmlEngine* e, QJSEngine*) -> QObject*
    {
      return &qt::qml_singleton_device::instance();
    });
    qmlRegisterType<qt::qml_node>("org.ossia", 1, 0, "OssiaNode");
    qmlRegisterType<qt::qml_device>("org.ossia", 1, 0, "OssiaDevice");
    qmlRegisterType<qt::qml_property>("org.ossia", 1, 0, "OssiaPropertyBase");
    //qmlRegisterType<qt::qml_impulse>("org.ossia", 1, 0, "OssiaImpulse");

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
