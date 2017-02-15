#pragma once
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia-qt/js_utilities.hpp>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QtQml/QQmlProperty>

namespace ossia
{
namespace qt
{
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

    m_protocol.push(*this);
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

  void qtImpulse() { m_protocol.push(*this); }


  void setValue_slot(const value&);

signals:
  void setValue_sig(const value&);

private:
  std::unique_ptr<node_base> makeChild(const std::string& name) final override;
  void removingChild(node_base&) final override;

  QObject& m_obj;
  QMetaProperty m_prop;
};
}
}
