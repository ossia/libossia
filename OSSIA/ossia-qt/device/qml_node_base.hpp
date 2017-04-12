#pragma once
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QQuickItem>
#include <boost/any.hpp>
#include <nano_observer.hpp>
namespace ossia
{
namespace net { class node_base; }
namespace qt
{
class qml_device;
class qml_node_base
  : public QQuickItem
  , public Nano::Observer
{
  Q_OBJECT
  Q_PROPERTY(QString node READ node WRITE setNode NOTIFY nodeChanged)
  Q_PROPERTY(QString path READ path NOTIFY pathChanged)
  Q_PROPERTY(qml_device* device READ device WRITE setDevice NOTIFY deviceChanged)
  Q_PROPERTY(QVariantMap extended READ extended WRITE setExtended NOTIFY extendedChanged)
  Q_PROPERTY(qml_node_base* parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged)

public:
  qml_node_base(QQuickItem* parent = nullptr);
  ~qml_node_base();

  QString node() const;
  qml_device* device() const;
  ossia::net::node_base* ossiaNode();

  QString path() const;

  virtual void resetNode() = 0;
  QVariantMap extended() const;

  qml_node_base* parentNode() const;

public slots:
  void setNode(QString node);
  virtual void setDevice(qml_device* device);
  void setExtended(QVariantMap extended);

  void setParentNode(qml_node_base* parentNode);

signals:
  void nodeChanged(QString node);
  void deviceChanged(qml_device* device);
  void pathChanged(QString path);
  void extendedChanged(QVariantMap extended);

  void parentNodeChanged(qml_node_base* parentNode);

protected:
  QVariant anyToVariant(const boost::any&) const;
  void setPath(QString str);
  ossia::net::node_base& findClosestParent(
      QObject* obj,
      ossia::net::node_base& root);

  QString m_node;
  QString m_userRequestedNode;
  qml_device* m_device{};
  qml_node_base* m_parentNode{};
  ossia::net::node_base* m_ossia_node{};
  QString m_path;
};

}
}
