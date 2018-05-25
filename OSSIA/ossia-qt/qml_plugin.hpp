#pragma once
#include <ossia/detail/config.hpp>
#include <QQmlExtensionPlugin>
#include <wobjectdefs.h>
#include <QQmlExtensionInterface>
#include <ossia_export.h>
namespace ossia
{
namespace qt
{
#if defined(OSSIA_DISABLE_QT_PLUGIN)
class OSSIA_EXPORT qml_plugin
{
public:
  static void reg(const char* uri);
};
#else
class OSSIA_EXPORT qml_plugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
  static void reg(const char* uri);

  void registerTypes(const char* uri) override;
};
#endif
}
}
