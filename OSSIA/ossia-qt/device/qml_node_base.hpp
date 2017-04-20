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
  Q_PROPERTY(QString node READ node WRITE setNode NOTIFY nodeChanged FINAL)
  Q_PROPERTY(QString path READ path NOTIFY pathChanged FINAL)
  Q_PROPERTY(QObject* device READ device WRITE setDevice NOTIFY deviceChanged)
  Q_PROPERTY(qml_node_base* parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged FINAL)

  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged FINAL)
  Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged FINAL)
  Q_PROPERTY(qint32 priority READ priority WRITE setPriority NOTIFY priorityChanged FINAL)
  Q_PROPERTY(qint32 refreshRate READ refreshRate WRITE setRefreshRate NOTIFY refreshRateChanged FINAL)
  Q_PROPERTY(double stepSize READ stepSize WRITE setStepSize NOTIFY stepSizeChanged FINAL)
  Q_PROPERTY(QVariant defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged FINAL)
  Q_PROPERTY(bool critical READ critical WRITE setCritical NOTIFY criticalChanged FINAL)
  Q_PROPERTY(bool zombie READ zombie FINAL)


public:
  qml_node_base(QQuickItem* parent = nullptr);
  ~qml_node_base();

  QString node() const;
  QObject* device() const;
  ossia::net::node_base* ossiaNode();

  QString path() const;

  virtual void resetNode() = 0;

  qml_node_base* parentNode() const;

  qint32 priority() const;
  QString description() const;
  QStringList tags() const;
  qint32 refreshRate() const;
  double stepSize() const;
  QVariant defaultValue() const;
  bool critical() const;
  bool zombie() const;

public slots:
  void setNode(QString node);
  virtual void setDevice(QObject* device);

  void setParentNode(qml_node_base* parentNode);
  void setPriority(qint32 priority);
  void setDescription(QString description);
  void setTags(QStringList tags);
  void setRefreshRate(qint32 refreshRate);
  void setStepSize(double stepSize);
  void setDefaultValue(QVariant defaultValue);
  void setCritical(bool critical);

signals:
  void nodeChanged(QString node);
  void deviceChanged(qml_device* device);
  void pathChanged(QString path);

  void parentNodeChanged(qml_node_base* parentNode);

  void priorityChanged(qint32 priority);
  void descriptionChanged(QString description);
  void tagsChanged(QStringList tags);
  void refreshRateChanged(qint32 refreshRate);
  void stepSizeChanged(double stepSize);
  void defaultValueChanged(QVariant defaultValue);
  void criticalChanged(bool critical);

protected:
  void applyNodeAttributes();

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

  // Attributes :

  QString m_description;
  QStringList m_tags;
  qint32 m_priority{};
  qint32 m_refreshRate{};
  double m_stepSize{};
  QVariant m_defaultValue;
  bool m_critical{};
  bool m_zombie{};
};

}
}
