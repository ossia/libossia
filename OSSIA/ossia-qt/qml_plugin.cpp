#include "qml_plugin.hpp"
#include <QQmlEngine>
#include <qqml.h>
#include <ossia-qt/js_utilities.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/device/qml_model_property.hpp>
#include <ossia/context.hpp>

namespace ossia
{
namespace qt
{

void qml_plugin::reg(const char* uri)
{
  // See ossia_global_init
  qmlRegisterSingletonType<qt::qml_context>(uri, 1, 0, "Context",
                                            [] (QQmlEngine* e, QJSEngine*) -> QObject*
  { return new qt::qml_context; });

  qmlRegisterSingletonType<qt::qml_singleton_device>(uri, 1, 0, "SingleDevice",
                                                     [] (QQmlEngine* e, QJSEngine*) -> QObject*
  { return &qt::qml_singleton_device::instance(); });

  qmlRegisterType<qt::qml_node>(uri, 1, 0, "NodeImpl");
  qmlRegisterType<qt::qml_device>(uri, 1, 0, "Device");
  qmlRegisterType<qt::qml_property>(uri, 1, 0, "Property");
  qmlRegisterType<qt::qml_model_property>(uri, 1, 0, "Instances");
}

void qml_plugin::registerTypes(const char* uri)
{
  // Note : it will be registered twice. But maybe that's not a problem...
  reg(uri);
  static ossia::context c;
}

}
}
