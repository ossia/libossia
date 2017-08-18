#pragma once
#include <ossia/network/base/protocol.hpp>
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
  Q_OBJECT
public:
  qt_property_node(
      QObject& obj, QMetaProperty p, ossia::net::device_base& aDevice,
      node_base& aParent);

  void set_value_quiet(const ossia::value&) final override;
  void set_value_quiet(ossia::value&&) final override;
  ossia::net::parameter_base* get_address() const final override;
  ossia::net::parameter_base*
  create_address(ossia::val_type type) final override;
  bool remove_address() final override;

  template <typename T>
  void qtValueChanged(const T& val)
  {
    ossia::net::generic_parameter::set_value_quiet(qt_to_ossia{}(val));

    m_protocol.push(*this);
  }

private slots:
  void qtBoolValueChanged(bool v)
  {
    qtValueChanged(v);
  }
  void qtTimeValueChanged(QTime v)
  {
    qtValueChanged(v);
  }
  void qtIntValueChanged(int v)
  {
    qtValueChanged(v);
  }
  void qtUIntValueChanged(quint32 v)
  {
    qtValueChanged(v);
  }
  void qtULongLongValueChanged(qulonglong v)
  {
    qtValueChanged(v);
  }
  void qtCharValueChanged(QChar v)
  {
    qtValueChanged(v);
  }
  void qtStringValueChanged(QString v)
  {
    qtValueChanged(v);
  }
  void qtByteArrayValueChanged(QByteArray v)
  {
    qtValueChanged(v);
  }
  void qtDoubleValueChanged(double v)
  {
    qtValueChanged(v);
  }
  void qtColorValueChanged(QColor v)
  {
    qtValueChanged(v);
  }
  void qtPointValueChanged(QPoint v)
  {
    qtValueChanged(v);
  }
  void qtPointFValueChanged(QPointF v)
  {
    qtValueChanged(v);
  }
  void qtVector2DValueChanged(QVector2D v)
  {
    qtValueChanged(v);
  }
  void qtVector3DValueChanged(QVector3D v)
  {
    qtValueChanged(v);
  }
  void qtVector4DValueChanged(QVector4D v)
  {
    qtValueChanged(v);
  }
  void qtQuaternionValueChanged(QQuaternion v)
  {
    qtValueChanged(v);
  }
  void qtLineValueChanged(QLine v)
  {
    qtValueChanged(v);
  }
  void qtLineFValueChanged(QLineF v)
  {
    qtValueChanged(v);
  }
  void qtRectValueChanged(QRectF v)
  {
    qtValueChanged(v);
  }
  void qtRectFValueChanged(QRectF v)
  {
    qtValueChanged(v);
  }
  void qtSizeValueChanged(QSize v)
  {
    qtValueChanged(v);
  }
  void qtSizeFValueChanged(QSizeF v)
  {
    qtValueChanged(v);
  }
  void qtListValueChanged(QVariantList v)
  {
    qtValueChanged(v);
  }
  void qtStringListValueChanged(QStringList v)
  {
    qtValueChanged(v);
  }
  void qtDateValueChanged(QDate v)
  {
    qtValueChanged(v);
  }

  void qtImpulse()
  {
    m_protocol.push(*this);
  }

  void setValue_slot(const ossia::value&);

signals:
  void setValue_sig(const ossia::value&);

private:
  std::unique_ptr<node_base>
  make_child(const std::string& name) final override;
  void removing_child(node_base&) final override;

  QObject& m_obj;
  QMetaProperty m_prop;
};
}
}
