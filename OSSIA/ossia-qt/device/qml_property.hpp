#pragma once
#include <QObject>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <ossia/network/base/address.hpp>
#include <ossia-qt/device/qml_node_base.hpp>
#include <ossia-qt/qml_context.hpp>
namespace ossia
{
namespace qt
{
class qml_node;
class qml_device;

class qml_property :
    public qml_node_base,
    public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)

  Q_PROPERTY(QVariant min READ min WRITE setMin NOTIFY minChanged FINAL)
  Q_PROPERTY(QVariant max READ max WRITE setMax NOTIFY maxChanged FINAL)
  Q_PROPERTY(QVariantList values READ values WRITE setValues NOTIFY valuesChanged FINAL)
  Q_PROPERTY(QString unit READ unit WRITE setUnit NOTIFY unitChanged FINAL)
  Q_PROPERTY(qml_context::val_type valueType READ valueType WRITE setValueType NOTIFY valueTypeChanged FINAL)
  Q_PROPERTY(qml_context::access_mode access READ access WRITE setAccess NOTIFY accessChanged FINAL)
  Q_PROPERTY(qml_context::bounding_mode bounding READ bounding WRITE setBounding NOTIFY boundingChanged FINAL)
  Q_PROPERTY(qml_context::repetition_filter filterRepetitions READ filterRepetitions WRITE setFilterRepetitions NOTIFY filterRepetitionsChanged FINAL)
public:
  qml_property(QQuickItem *parent = nullptr);
  ~qml_property();

  void setTarget(const QQmlProperty &prop) override;

  void resetNode() override;
  void setDevice(qml_device* device) override;

  void updateQtValue();
  qml_context::val_type valueType() const;

  qml_context::access_mode access() const;

  qml_context::bounding_mode bounding() const;

  qml_context::repetition_filter filterRepetitions() const;

  QVariant min() const
  {
    return m_min;
  }

  QVariant max() const
  {
    return m_max;
  }

  QVariantList values() const
  {
    return m_values;
  }

  QString unit() const
  {
    return m_unit;
  }

  signals:
  void setValue_sig(const value&);

  void valueTypeChanged(qml_context::val_type valueType);

  void accessChanged(qml_context::access_mode access);

  void boundingChanged(qml_context::bounding_mode bounding);

  void filterRepetitionsChanged(qml_context::repetition_filter filterRepetitions);

  void minChanged(QVariant min);

  void maxChanged(QVariant max);

  void valuesChanged(QVariantList values);

  void unitChanged(QString unit);

  public slots:
  void qtVariantChanged();
  void setValue_slot(const value&);

  void setValueType(qml_context::val_type valueType);

  void setAccess(qml_context::access_mode access);

  void setBounding(qml_context::bounding_mode bounding);

  void setFilterRepetitions(qml_context::repetition_filter filterRepetitions);

  void setMin(QVariant min)
  {
    if (m_min == min)
      return;

    m_min = min;
    emit minChanged(min);
  }

  void setMax(QVariant max)
  {
    if (m_max == max)
      return;

    m_max = max;
    emit maxChanged(max);
  }

  void setValues(QVariantList values)
  {
    if (m_values == values)
      return;

    m_values = values;
    emit valuesChanged(values);
  }

  void setUnit(QString unit)
  {
    if (m_unit == unit)
      return;

    m_unit = unit;
    emit unitChanged(unit);
  }

  private:
  void setupAddress(bool reading);
  void on_node_deleted(const ossia::net::node_base&);

  QQmlProperty m_targetProperty;
  ossia::net::address_base* m_address{};
  qml_context::val_type m_valueType;
  qml_context::access_mode m_access;
  qml_context::bounding_mode m_bounding;
  qml_context::repetition_filter m_filterRepetitions;
  QVariant m_min;
  QVariant m_max;
  QVariantList m_values;
  QString m_unit;
};

}
}
