#pragma once
#include <QQmlExtensionPlugin>
#include <ossia_export.h>
namespace ossia
{
namespace qt
{

class OSSIA_EXPORT qml_plugin final : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    static void reg(const char* uri);

    void registerTypes(const char *uri) override;
};

}
}
