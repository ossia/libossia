#pragma once
#include <QObject>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <ossia/network/base/address.hpp>
#include <ossia-qt/device/qml_node_base.hpp>
namespace ossia
{
namespace qt
{
class qml_node;
class qml_device;

class qml_property :
    public qml_node_base,
    public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)
public:
  qml_property(QObject *parent = nullptr);

  void setTarget(const QQmlProperty &prop) override;

signals:
  void setValue_sig(const value&);

public slots:
  void qtVariantChanged();
  void setValue_slot(const value&);

private:
  void resetNode() override;
  void setupAddress();

  QQmlProperty m_targetProperty;
  ossia::net::address_base* m_address{};
  QMetaObject::Connection m_conn;
};

}
}
