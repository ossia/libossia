#pragma once
#include <ossia/network/base/address.hpp>
#include <QObject>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <ossia-qt/device/qml_node_base.hpp>
#include <ossia-qt/qml_context.hpp>
namespace ossia
{
namespace qt
{
class qml_node;
class qml_device;

class OSSIA_EXPORT qml_property : public qml_property_base,
                                  public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)

  Q_PROPERTY(QString unit READ unit WRITE setUnit NOTIFY unitChanged RESET
                 resetUnit FINAL)
  Q_PROPERTY(
      ossia::qt::qml_val_type::val_type valueType READ valueType WRITE
          setValueType NOTIFY valueTypeChanged RESET resetValueType FINAL)
  Q_PROPERTY(ossia::qt::qml_access_mode::access_mode access READ access WRITE
                 setAccess NOTIFY accessChanged RESET resetAccess FINAL)
  Q_PROPERTY(
      ossia::qt::qml_bounding_mode::bounding_mode bounding READ bounding WRITE
          setBounding NOTIFY boundingChanged RESET resetBounding FINAL)
  Q_PROPERTY(
      ossia::qt::qml_rep_filter::repetition_filter filterRepetitions READ
          filterRepetitions WRITE setFilterRepetitions NOTIFY
              filterRepetitionsChanged RESET resetFilterRepetitions FINAL)

  Q_PROPERTY(QVariant min READ min WRITE setMin NOTIFY minChanged RESET
                 resetMin FINAL)
  Q_PROPERTY(QVariant max READ max WRITE setMax NOTIFY maxChanged RESET
                 resetMax FINAL)
  Q_PROPERTY(QVariantList values READ values WRITE setValues NOTIFY
                 valuesChanged RESET resetValues FINAL)
public:
  qml_property(QQuickItem* parent = nullptr);
  ~qml_property();

  void setTarget(const QQmlProperty& prop) override;
  void resetNode() override;
  void setDevice(QObject* device) override;

  qml_val_type::val_type valueType() const;
  qml_access_mode::access_mode access() const;
  qml_bounding_mode::bounding_mode bounding() const;
  qml_rep_filter::repetition_filter filterRepetitions() const;
  QVariant min() const;
  QVariant max() const;
  QVariantList values() const;
  QString unit() const;

signals:
  void setValue_sig(const ossia::value&);

  void valueTypeChanged(qml_val_type::val_type valueType);
  void accessChanged(qml_access_mode::access_mode access);
  void boundingChanged(qml_bounding_mode::bounding_mode bounding);
  void filterRepetitionsChanged(
      qml_rep_filter::repetition_filter filterRepetitions);
  void minChanged(QVariant min);
  void maxChanged(QVariant max);
  void valuesChanged(QVariantList values);
  void unitChanged(QString unit);

public slots:
  void qtVariantChanged();
  void setValue_slot(const value&);

  void setValueType(qml_val_type::val_type valueType);
  void setAccess(qml_access_mode::access_mode access);
  void setBounding(qml_bounding_mode::bounding_mode bounding);
  void
  setFilterRepetitions(qml_rep_filter::repetition_filter filterRepetitions);
  void setMin(QVariant min);
  void setMax(QVariant max);
  void setValues(QVariantList values);
  void setUnit(QString unit);

  void resetValueType()
  {
    if (m_valueType)
    {
      m_valueType = ossia::none;
    }
  }
  void resetAccess()
  {
    if (m_access)
    {
      m_access = ossia::none;
    }
  }
  void resetBounding()
  {
    if (m_bounding)
    {
      m_bounding = ossia::none;
    }
  }
  void resetFilterRepetitions()
  {
    if (m_filterRepetitions)
    {
      m_filterRepetitions = ossia::none;
    }
  }
  void resetMin()
  {
    setMin(QVariant{});
  }
  void resetMax()
  {
    setMax(QVariant{});
  }
  void resetValues()
  {
    if (m_values)
    {
      m_values = ossia::none;
    }
  }
  void resetUnit()
  {
    if (m_unit)
    {
      m_values = ossia::none;
    }
  }

private:
  void setupAddress(bool reading);
  void updateDomain();

  QQmlProperty m_targetProperty;
  optional<qml_val_type::val_type> m_valueType{};
  optional<qml_access_mode::access_mode> m_access{};
  optional<qml_bounding_mode::bounding_mode> m_bounding{};
  optional<qml_rep_filter::repetition_filter> m_filterRepetitions{};
  QVariant m_min{};
  QVariant m_max{};
  optional<QVariantList> m_values{};
  optional<QString> m_unit{};
  bool m_updatingFromSetValue{};
};
}
}
