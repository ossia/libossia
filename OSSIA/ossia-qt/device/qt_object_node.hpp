#pragma once
#include <ossia/network/base/protocol.hpp>
#include <wobjectdefs.h>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QtQml/QQmlProperty>
#include <ossia-qt/js_utilities.hpp>

namespace ossia
{
namespace qt
{

class OSSIA_EXPORT qt_object_node final : public QObject,
                                          public ossia::net::generic_node_base

{
  W_OBJECT(qt_object_node)
public:
  qt_object_node(QObject& obj, ossia::net::device_base& device);
  qt_object_node(
      QObject& obj, ossia::net::device_base& device, node_base& aParent);

  ossia::net::parameter_base* get_parameter() const final override;
  ossia::net::parameter_base*
  create_parameter(ossia::val_type type) final override;
  bool remove_parameter() final override;

  QObject& object() const
  {
    return mObject;
  }

private:
  void init(QObject&);
  void childEvent(QChildEvent* event) override;

  std::unique_ptr<node_base>
  make_child(const std::string& name) final override;
  void removing_child(node_base&) final override;

  std::unique_ptr<ossia::net::parameter_base> mAddress;
  QObject& mObject;
};
}
}
