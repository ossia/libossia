#pragma once
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QtQml/QQmlProperty>

namespace ossia
{
namespace qt
{

class OSSIA_EXPORT qt_object_node final :
    public QObject,
    public ossia::net::generic_node_base

{
  Q_OBJECT
public:
  qt_object_node(
      QObject& obj,
      ossia::net::device_base& device);
  qt_object_node(
      QObject& obj,
      ossia::net::device_base& device,
      node_base& aParent);

  ossia::net::address_base* get_address() const final override;
  ossia::net::address_base* create_address(ossia::val_type type) final override;
  bool remove_address() final override;

  QObject& object() const { return mObject; }
private:
  void init(QObject&);
  void childEvent(QChildEvent* event) override;

  std::unique_ptr<node_base> make_child(const std::string& name) final override;
  void removing_child(node_base&) final override;

  std::unique_ptr<ossia::net::address_base> mAddress;
  QObject& mObject;

};


}
}
