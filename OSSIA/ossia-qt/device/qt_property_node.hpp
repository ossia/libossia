#pragma once
#include <ossia/network/base/protocol.hpp>
#include <wobjectdefs.h>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QtQml/QQmlProperty>
#include <ossia-qt/js_utilities.hpp>

namespace ossia
{
namespace qt
{
class OSSIA_EXPORT qt_property_node final
    : public QObject,
      public ossia::net::generic_node_base,
      public ossia::net::generic_parameter
{
  W_OBJECT(qt_property_node)
public:
  qt_property_node(
      QObject& obj, QMetaProperty p, ossia::net::device_base& aDevice,
      node_base& aParent);

  void set_value_quiet(const ossia::value&) final override;
  void set_value_quiet(ossia::value&&) final override;
  ossia::net::parameter_base* get_parameter() const final override;
  ossia::net::parameter_base*
  create_parameter(ossia::val_type type) final override;
  bool remove_parameter() final override;

  template <typename T>
  void qtValueChanged(const T& val)
  {
    ossia::net::generic_parameter::set_value_quiet(qt_to_ossia{}(val));

    m_protocol.push(*this);
  }

private:
  void qtBoolValueChanged(bool v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtBoolValueChanged)
  void qtTimeValueChanged(QTime v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtTimeValueChanged)
  void qtIntValueChanged(int v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtIntValueChanged)
  void qtUIntValueChanged(quint32 v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtUIntValueChanged)
  void qtULongLongValueChanged(qulonglong v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtULongLongValueChanged)
  void qtCharValueChanged(QChar v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtCharValueChanged)
  void qtStringValueChanged(QString v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtStringValueChanged)
  void qtByteArrayValueChanged(QByteArray v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtByteArrayValueChanged)
  void qtDoubleValueChanged(double v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtDoubleValueChanged)
  void qtColorValueChanged(QColor v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtColorValueChanged)
  void qtPointValueChanged(QPoint v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtPointValueChanged)
  void qtPointFValueChanged(QPointF v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtPointFValueChanged)
  void qtVector2DValueChanged(QVector2D v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtVector2DValueChanged)
  void qtVector3DValueChanged(QVector3D v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtVector3DValueChanged)
  void qtVector4DValueChanged(QVector4D v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtVector4DValueChanged)
  void qtQuaternionValueChanged(QQuaternion v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtQuaternionValueChanged)
  void qtLineValueChanged(QLine v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtLineValueChanged)
  void qtLineFValueChanged(QLineF v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtLineFValueChanged)
  void qtRectValueChanged(QRectF v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtRectValueChanged)
  void qtRectFValueChanged(QRectF v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtRectFValueChanged)
  void qtSizeValueChanged(QSize v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtSizeValueChanged)
  void qtSizeFValueChanged(QSizeF v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtSizeFValueChanged)
  void qtListValueChanged(QVariantList v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtListValueChanged)
  void qtStringListValueChanged(QStringList v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtStringListValueChanged)
  void qtDateValueChanged(QDate v)
  {
    qtValueChanged(v);
  }; W_SLOT(qtDateValueChanged)

  void qtImpulse()
  {
    m_protocol.push(*this);
  }; W_SLOT(qtImpulse)

  void setValue_slot(const ossia::value&); W_SLOT(setValue_slot);

public:
  void setValue_sig(const ossia::value& arg_1) E_SIGNAL(OSSIA_EXPORT, setValue_sig, arg_1);

private:
  std::unique_ptr<node_base>
  make_child(const std::string& name) final override;
  void removing_child(node_base&) final override;

  QObject& m_obj;
  QMetaProperty m_prop;
};
}
}
