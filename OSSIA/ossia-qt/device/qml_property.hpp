#pragma once


#include <QQmlProperty>
#include <QObject>
#include <QQmlPropertyValueSource>
#include <QtQml>

#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_address.hpp>

namespace ossia
{
namespace qt
{

class qml_property :
    public QObject,
    public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)
  Q_PROPERTY(QString node READ node WRITE setNode NOTIFY nodeChanged)
  Q_PROPERTY(QObject* device READ device WRITE setDevice NOTIFY deviceChanged)
public:
  qml_property(QObject *parent = nullptr);

  void setTarget(const QQmlProperty &prop) override;

  QString node() const;
  QObject* device() const;

public slots:
  void setNode(QString node);
  void setDevice(QObject* device);

signals:
  void nodeChanged(QString node);
  void deviceChanged(QObject* device);

private:
  void resetNode();
  void setupAddress();
  QQmlProperty m_targetProperty;
  QString m_node;
  QObject* m_device{};
  ossia::net::node_base* m_ossia_node{};
  ossia::net::address_base* m_address{};
  int m_type{};
};

}
}
