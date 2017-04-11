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
  qml_property(QQuickItem *parent = nullptr);
  ~qml_property();

  void setTarget(const QQmlProperty &prop) override;

  void resetNode() override;
  void setDevice(QObject* device) override;

  void updateQtValue();
signals:
  void setValue_sig(const value&);

public slots:
  void qtVariantChanged();
  void setValue_slot(const value&);

private:
  void setupAddress(bool reading);
  void on_node_deleted(const ossia::net::node_base&);

  QQmlProperty m_targetProperty;
  ossia::net::address_base* m_address{};
};

}
}
