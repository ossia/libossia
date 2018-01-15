#pragma once
#include <ossia/detail/optional.hpp>
#include <ossia/network/base/parameter.hpp>
#include <QObject>
#include <QQuickItem>
#include <QString>
#include <QVariantMap>
#include <boost/any.hpp>
#include <nano_observer.hpp>
#include <QPointer>
namespace ossia
{
namespace net
{
class node_base;
class parameter_base;
}
namespace qt
{
class qml_device;
class OSSIA_EXPORT qml_node_base : public QQuickItem, public Nano::Observer
{
  Q_OBJECT
  Q_PROPERTY(QString node READ node WRITE setNode NOTIFY nodeChanged FINAL)
  Q_PROPERTY(QString path READ path NOTIFY pathChanged FINAL)
  Q_PROPERTY(QObject* device READ device WRITE setDevice NOTIFY deviceChanged)
  Q_PROPERTY(qml_node_base* parentNode READ parentNode WRITE setParentNode
                 NOTIFY parentNodeChanged FINAL)

  Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY
                 descriptionChanged FINAL)
  Q_PROPERTY(QString extendedType READ extendedType WRITE setExtendedType
                 NOTIFY extendedTypeChanged FINAL)
  Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged FINAL)
  Q_PROPERTY(qreal priority READ priority WRITE setPriority NOTIFY
                 priorityChanged FINAL)
  Q_PROPERTY(qint32 refreshRate READ refreshRate WRITE setRefreshRate NOTIFY
                 refreshRateChanged FINAL)
  Q_PROPERTY(qreal stepSize READ stepSize WRITE setStepSize NOTIFY
                 stepSizeChanged FINAL)
  Q_PROPERTY(QVariant defaultValue READ defaultValue WRITE setDefaultValue
                 NOTIFY defaultValueChanged FINAL)
  Q_PROPERTY(bool critical READ critical WRITE setCritical NOTIFY
                 criticalChanged FINAL)
  Q_PROPERTY(bool hidden READ hidden WRITE setHidden NOTIFY hiddenChanged FINAL)
  Q_PROPERTY(bool disabled READ disabled WRITE setDisabled NOTIFY disabledChanged FINAL)
  Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged FINAL)

public:
  qml_node_base(QQuickItem* parent = nullptr);
  ~qml_node_base();

  QString node() const;
  QObject* device() const;
  ossia::net::node_base* ossiaNode();

  QString path() const;

  virtual void resetNode() = 0;

  qml_node_base* parentNode() const;

  qreal priority() const;
  QString description() const;
  QStringList tags() const;
  qint32 refreshRate() const;
  qreal stepSize() const;
  QVariant defaultValue() const;
  bool critical() const;
  bool hidden() const;
  bool disabled() const;
  bool muted() const;
  QString extendedType() const;

public Q_SLOTS:
  void setNode(QString node);
  virtual void setDevice(QObject* device);

  void setParentNode(qml_node_base* parentNode);
  void setPriority(qreal priority);
  void setDescription(QString description);
  void setTags(QStringList tags);
  void setExtendedType(QString extendedType);
  void setRefreshRate(qint32 refreshRate);
  void setStepSize(qreal stepSize);
  void setDefaultValue(QVariant defaultValue);
  void setCritical(bool critical);
  void setHidden(bool hidden);
  void setDisabled(bool disabled);
  void setMuted(bool muted);

signals:
  void nodeChanged(QString node);
  void deviceChanged(qml_device* device);
  void pathChanged(QString path);

  void parentNodeChanged(qml_node_base* parentNode);

  void priorityChanged(qreal priority);
  void descriptionChanged(QString description);
  void extendedTypeChanged(QString extendedType);
  void tagsChanged(QStringList tags);
  void refreshRateChanged(qint32 refreshRate);
  void stepSizeChanged(qreal stepSize);
  void defaultValueChanged(QVariant defaultValue);
  void criticalChanged(bool critical);
  void hiddenChanged(bool hidden);
  void disabledChanged(bool d);
  void mutedChanged(bool muted);

protected:
  void applyNodeAttributes();
  ossia::net::node_base& get_parent(QObject* obj, bool relative);

  void setPath(QString str);
  ossia::net::node_base&
  findClosestParent(QObject* obj, ossia::net::node_base& root);

  QString m_node;
  QString m_userRequestedNode;
  QPointer<qml_device> m_device{};
  QPointer<qml_node_base> m_parentNode{};
  ossia::net::node_base* m_ossia_node{};
  QString m_path;

  // Attributes :
  QString m_description;
  QString m_extendedType;
  QStringList m_tags;
  QVariant m_defaultValue;
  qreal m_priority{};
  qint32 m_refreshRate{};
  qreal m_stepSize{};
  bool m_critical{};
  bool m_hidden{};
  bool m_disabled{};
  bool m_muted{};


  bool m_was_destroyed{false};
};

class OSSIA_EXPORT qml_property_base : public qml_node_base
{
public:
  void on_node_deleted(const net::node_base& n);

protected:
  using qml_node_base::qml_node_base;
  void clearNode(bool reading);

  ossia::net::parameter_base* m_param{};
  ossia::optional<ossia::net::parameter_base::iterator> m_callback;
};
}
}
