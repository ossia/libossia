#pragma once
#include <ossia/detail/optional.hpp>
#include <wobjectdefs.h>
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
  W_OBJECT(qml_node_base)

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

public:
  void setNode(QString node); W_SLOT(setNode);
  virtual void setDevice(QObject* device); W_SLOT(setDevice);

  void setParentNode(qml_node_base* parentNode); W_SLOT(setParentNode);
  void setPriority(qreal priority); W_SLOT(setPriority);
  void setDescription(QString description); W_SLOT(setDescription);
  void setTags(QStringList tags); W_SLOT(setTags);
  void setExtendedType(QString extendedType); W_SLOT(setExtendedType);
  void setRefreshRate(qint32 refreshRate); W_SLOT(setRefreshRate);
  void setStepSize(qreal stepSize); W_SLOT(setStepSize);
  void setDefaultValue(QVariant defaultValue); W_SLOT(setDefaultValue);
  void setCritical(bool critical); W_SLOT(setCritical);
  void setHidden(bool hidden); W_SLOT(setHidden);
  void setDisabled(bool disabled); W_SLOT(setDisabled);
  void setMuted(bool muted); W_SLOT(setMuted);

public:
  void nodeChanged(QString node) W_SIGNAL(nodeChanged, node);
  void deviceChanged(qml_device* device) W_SIGNAL(deviceChanged, device);
  void pathChanged(QString path) W_SIGNAL(pathChanged, path);

  void parentNodeChanged(qml_node_base* parentNode) W_SIGNAL(parentNodeChanged, parentNode);

  void priorityChanged(qreal priority) W_SIGNAL(priorityChanged, priority);
  void descriptionChanged(QString description) W_SIGNAL(descriptionChanged, description);
  void extendedTypeChanged(QString extendedType) W_SIGNAL(extendedTypeChanged, extendedType);
  void tagsChanged(QStringList tags) W_SIGNAL(tagsChanged, tags);
  void refreshRateChanged(qint32 refreshRate) W_SIGNAL(refreshRateChanged, refreshRate);
  void stepSizeChanged(qreal stepSize) W_SIGNAL(stepSizeChanged, stepSize);
  void defaultValueChanged(QVariant defaultValue) W_SIGNAL(defaultValueChanged, defaultValue);
  void criticalChanged(bool critical) W_SIGNAL(criticalChanged, critical);
  void hiddenChanged(bool hidden) W_SIGNAL(hiddenChanged, hidden);
  void disabledChanged(bool d) W_SIGNAL(disabledChanged, d);
  void mutedChanged(bool muted) W_SIGNAL(mutedChanged, muted);

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

W_PROPERTY(bool, muted READ muted WRITE setMuted NOTIFY mutedChanged, W_Final)

W_PROPERTY(bool, disabled READ disabled WRITE setDisabled NOTIFY disabledChanged, W_Final)

W_PROPERTY(bool, hidden READ hidden WRITE setHidden NOTIFY hiddenChanged, W_Final)

W_PROPERTY(bool, critical READ critical WRITE setCritical NOTIFY criticalChanged, W_Final)

W_PROPERTY(QVariant, defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged, W_Final)

W_PROPERTY(qreal, stepSize READ stepSize WRITE setStepSize NOTIFY stepSizeChanged, W_Final)

W_PROPERTY(qint32, refreshRate READ refreshRate WRITE setRefreshRate NOTIFY refreshRateChanged, W_Final)

W_PROPERTY(qreal, priority READ priority WRITE setPriority NOTIFY priorityChanged, W_Final)

W_PROPERTY(QStringList, tags READ tags WRITE setTags NOTIFY tagsChanged, W_Final)

W_PROPERTY(QString, extendedType READ extendedType WRITE setExtendedType NOTIFY extendedTypeChanged, W_Final)

W_PROPERTY(QString, description READ description WRITE setDescription NOTIFY descriptionChanged, W_Final)

W_PROPERTY(qml_node_base*, parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged, W_Final)

W_PROPERTY(QObject*, device READ device WRITE setDevice NOTIFY deviceChanged)

W_PROPERTY(QString, path READ path NOTIFY pathChanged, W_Final)

W_PROPERTY(QString, node READ node WRITE setNode NOTIFY nodeChanged, W_Final)
};

class OSSIA_EXPORT qml_property_base : public qml_node_base
{
public:
  void on_node_deleted(const net::node_base& n);
  ~qml_property_base();

protected:
  using qml_node_base::qml_node_base;
  void clearNode(bool reading);

  ossia::net::parameter_base* m_param{};
  ossia::optional<ossia::net::parameter_base::iterator> m_callback;
};
}
}

W_REGISTER_ARGTYPE(ossia::qt::qml_node_base*)
