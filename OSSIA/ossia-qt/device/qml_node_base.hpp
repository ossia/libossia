#pragma once
#include <QObject>
#include <QString>

namespace ossia
{
namespace net { class node_base; }
namespace qt
{
class qml_device;
class qml_node_base :
    public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString node READ node WRITE setNode NOTIFY nodeChanged)
  Q_PROPERTY(QString path READ path NOTIFY pathChanged)
  Q_PROPERTY(QObject* device READ device WRITE setDevice NOTIFY deviceChanged)

  public:
  qml_node_base(QObject* parent = nullptr);
  QString node() const;
  QObject* device() const;
  ossia::net::node_base* ossiaNode() { return m_ossia_node; }

  QString path() const;


  virtual void resetNode() = 0;
  void reparentChildren();

public slots:
  void setNode(QString node);
  void setDevice(QObject* device);

signals:
  void nodeChanged(QString node);
  void deviceChanged(QObject* device);
  void pathChanged(QString path);

protected:
  void setPath(QString str);
  ossia::net::node_base& findClosestParent(
      QObject* obj,
      ossia::net::node_base& root);

  QString m_node;
  qml_device* m_device;
  ossia::net::node_base* m_ossia_node{};
  QString m_path;
};


}
}
