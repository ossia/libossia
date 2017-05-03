#pragma once
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia-qt/device/qt_object_node.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QtQml/QQmlProperty>
namespace ossia
{
namespace qt
{
class OSSIA_EXPORT qt_device final :
    public ossia::net::device_base
{
public:
  qt_device(
      QObject& obj,
      std::unique_ptr<ossia::net::protocol_base> protocol_base,
      std::string name);

  const ossia::net::node_base& get_root_node() const override;
  ossia::net::node_base& get_root_node() override;

private:
  qt_object_node m_root;
};


}
}
