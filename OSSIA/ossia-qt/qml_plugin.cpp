#include "qml_plugin.hpp"
#include <QQmlEngine>
#include <ossia-qt/js_utilities.hpp>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia/context.hpp>

namespace ossia
{
namespace qt
{

void qml_plugin::reg(const char* uri)
{
  // See ossia_global_init
  qmlRegisterUncreatableType<qt::qml_context>(uri, 1, 0, "Ossia", "");

  qmlRegisterSingletonType<qt::qml_singleton_device>(uri, 1, 0, "OssiaSingleDevice",
                                                     [] (QQmlEngine* e, QJSEngine*) -> QObject*
  { return &qt::qml_singleton_device::instance(); });

  qmlRegisterType<qt::qml_node>(uri, 1, 0, "OssiaNode");
  qmlRegisterType<qt::qml_device>(uri, 1, 0, "OssiaDevice");
  qmlRegisterType<qt::qml_property>(uri, 1, 0, "OssiaPropertyBase");
}

void qml_plugin::registerTypes(const char* uri)
{
  qDebug() << "REGISTERING on " << uri;
  // Note : it will be registered twice. But maybe that's not a problem...
  reg(uri);
  static ossia::context c;
}

}
}
