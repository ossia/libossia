#pragma once
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/base/protocol.hpp>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QTime>
#include <QPoint>
#include <QLineF>
#include <QRectF>
#include <QVariantList>
#include <QtGui/QColor>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <QtGui/QQuaternion>
namespace ossia
{
namespace qt
{
class OSSIA_EXPORT qt_object_node :
    public QObject,
    public ossia::net::generic_node_base

{
  Q_OBJECT
public:
  qt_object_node(
      QObject& obj,
      ossia::net::device_base& device);
  qt_object_node(
      QObject& obj,
      ossia::net::device_base& device,
      node_base& aParent);

  ossia::net::address_base* getAddress() const final override;
  ossia::net::address_base* createAddress(ossia::val_type type) final override;
  bool removeAddress() final override;

  QObject& object() const { return mObject; }
private:
  void init(QObject&);
  void childEvent(QChildEvent* event) override;

  std::unique_ptr<node_base> makeChild(const std::string& name) final override;
  void removingChild(node_base&) final override;

  std::unique_ptr<ossia::net::address_base> mAddress;
  QObject& mObject;

};

struct qt_to_ossia
{
  ossia::value operator()(bool v) { return v; }
  ossia::value operator()(QTime v) { return v.msec(); }
  ossia::value operator()(int v) { return v; }
  ossia::value operator()(quint32 v) { return (int)v; }
  ossia::value operator()(qulonglong v) { return (int)v; }
  ossia::value operator()(char v) { return v; }
  ossia::value operator()(QChar v) { return v.toLatin1(); }
  ossia::value operator()(const QString& v) { return v.toStdString(); }
  ossia::value operator()(const QByteArray& v) { return v.toStdString(); }
  ossia::value operator()(double v) { return v; }
  ossia::value operator()(QColor v) { return make_vec(v.alphaF(), v.redF(), v.greenF(), v.blueF()); }
  ossia::value operator()(QPoint v) { return make_vec(v.x(), v.y()); }
  ossia::value operator()(QPointF v) { return make_vec(v.x(), v.y()); }
  ossia::value operator()(QSize v) { return make_vec(v.width(), v.height()); }
  ossia::value operator()(QSizeF v) { return make_vec(v.width(), v.height()); }
  ossia::value operator()(QRect v) { return make_vec(v.x(), v.y(), v.width(), v.height()); }
  ossia::value operator()(QRectF v) { return make_vec(v.x(), v.y(), v.width(), v.height()); }
  ossia::value operator()(QLine v) { return make_vec(v.p1().x(), v.p1().y(), v.p2().x(), v.p2().y()); }
  ossia::value operator()(QLineF v) { return make_vec(v.p1().x(), v.p1().y(), v.p2().x(), v.p2().y()); }
  ossia::value operator()(QVector2D v) { return make_vec(v.x(), v.y()); }
  ossia::value operator()(QVector3D v) { return make_vec(v.x(), v.y(), v.z()); }
  ossia::value operator()(QVector4D v) { return make_vec(v.x(), v.y(), v.z(), v.w()); }
  ossia::value operator()(QQuaternion v) { return make_vec(v.scalar(), v.x(), v.y(), v.z()); }
  ossia::value operator()(QVariantList v) { return {}; }
  ossia::value operator()(QStringList v) { return {}; }
  ossia::value operator()(QDate v) { return v.toString().toStdString(); }
};

class OSSIA_EXPORT qt_property_node final :
    public QObject,
    public ossia::net::generic_node_base,
    public ossia::net::generic_address
{
  Q_OBJECT
public:
  qt_property_node(
      QObject& obj,
      QMetaProperty p,
      ossia::net::device_base& aDevice,
      node_base& aParent);

  void setValueQuiet(const ossia::value&) final override;
  ossia::net::address_base* getAddress() const final override;
  ossia::net::address_base* createAddress(ossia::val_type type) final override;
  bool removeAddress() final override;

  template<typename T>
  void qtValueChanged(const T& val)
  {
    ossia::net::generic_address::setValueQuiet(qt_to_ossia{}(val));

    mProtocol.push(*this);
  }

private slots:

  void qtBoolValueChanged(bool v) { qtValueChanged(v); }
  void qtTimeValueChanged(QTime v) { qtValueChanged(v); }
  void qtIntValueChanged(int v) { qtValueChanged(v); }
  void qtUIntValueChanged(quint32 v) { qtValueChanged(v); }
  void qtULongLongValueChanged(qulonglong v) { qtValueChanged(v); }
  void qtCharValueChanged(QChar v) { qtValueChanged(v); }
  void qtStringValueChanged(QString v) { qtValueChanged(v); }
  void qtByteArrayValueChanged(QByteArray v) { qtValueChanged(v); }
  void qtDoubleValueChanged(double v) { qtValueChanged(v); }
  void qtColorValueChanged(QColor v) { qtValueChanged(v); }
  void qtPointValueChanged(QPoint v) { qtValueChanged(v); }
  void qtPointFValueChanged(QPointF v) { qtValueChanged(v); }
  void qtVector2DValueChanged(QVector2D v) { qtValueChanged(v); }
  void qtVector3DValueChanged(QVector3D v) { qtValueChanged(v); }
  void qtVector4DValueChanged(QVector4D v) { qtValueChanged(v); }
  void qtQuaternionValueChanged(QQuaternion v) { qtValueChanged(v); }
  void qtLineValueChanged(QLine v) { qtValueChanged(v); }
  void qtLineFValueChanged(QLineF v) { qtValueChanged(v); }
  void qtRectValueChanged(QRectF v) { qtValueChanged(v); }
  void qtRectFValueChanged(QRectF v) { qtValueChanged(v); }
  void qtSizeValueChanged(QSize v) { qtValueChanged(v); }
  void qtSizeFValueChanged(QSizeF v) { qtValueChanged(v); }
  void qtListValueChanged(QVariantList v) { qtValueChanged(v); }
  void qtStringListValueChanged(QStringList v) { qtValueChanged(v); }
  void qtDateValueChanged(QDate v) { qtValueChanged(v); }

private:
  std::unique_ptr<node_base> makeChild(const std::string& name) final override;
  void removingChild(node_base&) final override;

  QObject& m_obj;
  QMetaProperty m_prop;
};

class OSSIA_EXPORT qt_device final :
    public ossia::net::device_base
{
public:
  qt_device(
      QObject& obj,
      std::unique_ptr<ossia::net::protocol_base> protocol_base,
      std::string name);

  const ossia::net::node_base& getRootNode() const override;
  ossia::net::node_base& getRootNode() override;


private:
  qt_object_node m_root;
};


}
}
