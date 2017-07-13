// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qml_plugin.hpp"
#include <ossia/context.hpp>
#include <QQmlEngine>
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_logger.hpp>
#include <ossia-qt/device/qml_model_property.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/device/qml_parameter.hpp>
#include <ossia-qt/device/qml_property.hpp>
#include <ossia-qt/device/qml_property_reader.hpp>
#include <ossia-qt/device/qml_signal.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <qqml.h>

namespace ossia
{
namespace qt
{

void qml_plugin::reg(const char* uri)
{
  // See ossia_global_init
  qmlRegisterUncreatableType<qt::qml_val_type>(
      uri, 1, 0, "Type", "Value type");
  qmlRegisterUncreatableType<qt::qml_access_mode>(
      uri, 1, 0, "Access", "Access mode");
  qmlRegisterUncreatableType<qt::qml_bounding_mode>(
      uri, 1, 0, "Bounding", "Bounding mode");
  qmlRegisterUncreatableType<qt::qml_rep_filter>(
      uri, 1, 0, "Repetitions", "Repetition filter");

  qmlRegisterSingletonType<qt::qml_singleton_device>(
      uri, 1, 0, "SingleDevice", [](QQmlEngine* e, QJSEngine*) -> QObject* {
        return &qt::qml_singleton_device::instance();
      });
  qmlRegisterSingletonType<qt::qml_logger>(
      uri, 1, 0, "Logger", [](QQmlEngine* e, QJSEngine*) -> QObject* {
        return &qt::qml_logger::instance();
      });

  qmlRegisterType<qt::qml_node>(uri, 1, 0, "NodeImpl");
  qmlRegisterType<qt::qml_device>(uri, 1, 0, "Device");
  qmlRegisterType<qt::qml_property>(uri, 1, 0, "Property");
  qmlRegisterType<qt::qml_parameter>(uri, 1, 0, "Parameter");
  qmlRegisterType<qt::qml_signal>(uri, 1, 0, "Signal");
  qmlRegisterType<qt::qml_property_reader>(uri, 1, 0, "ReaderImpl");
  qmlRegisterType<qt::qml_property_writer>(uri, 1, 0, "WriterImpl");
  qmlRegisterType<qt::qml_binding>(uri, 1, 0, "BindingImpl");
  qmlRegisterType<qt::qml_callback>(uri, 1, 0, "CallbackImpl");
  qmlRegisterType<qt::qml_model_property>(uri, 1, 0, "Instances");
}
#if !defined(OSSIA_DISABLE_QT_PLUGIN)
void qml_plugin::registerTypes(const char* uri)
{
  // Note : it will be registered twice. But maybe that's not a problem...
  reg(uri);
  static ossia::context c;
}
#endif
}
}
