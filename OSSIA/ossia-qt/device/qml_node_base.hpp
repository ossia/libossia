#pragma once
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QQuickItem>
#include <boost/any.hpp>

namespace ossia
{
namespace net { class node_base; }
namespace qt
{
class qml_device;
class qml_node_base :
    public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(QString node READ node WRITE setNode NOTIFY nodeChanged)
  Q_PROPERTY(QString path READ path NOTIFY pathChanged)
  Q_PROPERTY(QObject* device READ device WRITE setDevice NOTIFY deviceChanged)
  Q_PROPERTY(QVariantMap extended READ extended WRITE setExtended NOTIFY extendedChanged)

public:
  qml_node_base(QQuickItem* parent = nullptr);
  ~qml_node_base();

  QString node() const;
  QObject* device() const;
  ossia::net::node_base* ossiaNode() { return m_ossia_node; }

  QString path() const;


  virtual void resetNode(bool recursive = true) = 0;
  void reparentChildren();
  QVariantMap extended() const;

public slots:
  void setNode(QString node);
  void setDevice(QObject* device);
  void setExtended(QVariantMap extended);

signals:
  void nodeChanged(QString node);
  void deviceChanged(QObject* device);
  void pathChanged(QString path);
  void extendedChanged(QVariantMap extended);

protected:
  QVariant anyToVariant(const boost::any&) const;
  void setPath(QString str);
  ossia::net::node_base& findClosestParent(
      QObject* obj,
      ossia::net::node_base& root);

  QString m_node;
  QString m_userRequestedNode;
  qml_device* m_device{};
  ossia::net::node_base* m_ossia_node{};
  QString m_path;
};

}
}
