#pragma once
#include <QObject>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <ossia/network/base/address.hpp>
#include <ossia-qt/device/qml_node_base.hpp>
#include <ossia-qt/qml_context.hpp>
namespace ossia
{
namespace qt
{
class qml_node;
class qml_device;
class qml_property_context : public QObject
{
  Q_OBJECT
public:
  qml_property_context(
      QQmlProperty& p,
      ossia::net::address_base& addr,
      qml_device& dev,
      QObject* parent):
    QObject{parent},
    targetProperty{p}, address{addr}, device{dev}
  {
    if(targetProperty.hasNotifySignal())
    {
      targetProperty.connectNotifySignal(this, SLOT(qtVariantChanged()));
    }
  }

  QQmlProperty& targetProperty;
  ossia::net::address_base& address;
  qml_device& device;

public slots:
  void qtVariantChanged();
};

class qml_property_reader :
    public qml_node_base,
    public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)

public:
  qml_property_reader(QQuickItem *parent = nullptr);
  ~qml_property_reader();

  void setTarget(const QQmlProperty &prop) override;
  void setDevice(QObject* device) override;
  void resetNode() override;
  void updateQtValue();

private:
  void setupAddress(bool reading);
  void on_node_deleted(const ossia::net::node_base&);
  void clearNode();

  QQmlProperty m_targetProperty;
  ossia::net::address_base* m_address{};
  qml_property_context* m_propCtx{};
};

}
}
