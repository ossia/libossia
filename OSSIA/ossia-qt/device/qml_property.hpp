#pragma once


#include <QQmlProperty>
#include <QObject>
#include <QQmlPropertyValueSource>
#include <QtQml>

#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/generic/generic_address.hpp>

#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/js_utilities.hpp>
namespace ossia
{
namespace qt
{
class qml_node;
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

class qml_node : public qml_node_base
{
  Q_OBJECT
public:
  using qml_node_base::qml_node_base;

private:
   void resetNode() override;
};

class qml_property :
    public qml_node_base,
    public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)
public:
  qml_property(QObject *parent = nullptr);

  void setTarget(const QQmlProperty &prop) override;


  template<typename T>
  void qtValueChanged(const T& val)
  {
    if(m_address)
    {
      m_address->setValueQuiet(qt_to_ossia{}(val));
      m_device->device().getProtocol().push(*m_address);
    }
  }

public slots:
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

  void qtVariantChanged() { qtValueChanged(m_targetProperty.read()); }
private:
  void resetNode() override;
  void setupAddress();

  QQmlProperty m_targetProperty;
  ossia::net::address_base* m_address{};
  QMetaObject::Connection m_conn;
};

}
}
