#pragma once
#include <ossia/network/base/parameter.hpp>
#include <wobjectdefs.h>
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
  W_OBJECT(qml_property)
  W_INTERFACE(QQmlPropertyValueSource)

public:
  qml_property(QQuickItem* parent = nullptr);
  ~qml_property() override;

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

public:
  void setValue_sig(const ossia::value& arg_1) E_SIGNAL(OSSIA_EXPORT, setValue_sig, arg_1);

  void valueTypeChanged(qml_val_type::val_type valueType) E_SIGNAL(OSSIA_EXPORT, valueTypeChanged, valueType);
  void accessChanged(qml_access_mode::access_mode access) E_SIGNAL(OSSIA_EXPORT, accessChanged, access);
  void boundingChanged(qml_bounding_mode::bounding_mode bounding) E_SIGNAL(OSSIA_EXPORT, boundingChanged, bounding);
  void filterRepetitionsChanged(
      qml_rep_filter::repetition_filter filterRepetitions) E_SIGNAL(OSSIA_EXPORT, filterRepetitionsChanged, filterRepetitions);
  void minChanged(QVariant min) E_SIGNAL(OSSIA_EXPORT, minChanged, min);
  void maxChanged(QVariant max) E_SIGNAL(OSSIA_EXPORT, maxChanged, max);
  void valuesChanged(QVariantList values) E_SIGNAL(OSSIA_EXPORT, valuesChanged, values);
  void unitChanged(QString unit) E_SIGNAL(OSSIA_EXPORT, unitChanged, unit);

public:
  void qtVariantChanged(); W_SLOT(qtVariantChanged);
  void setValue_slot(const value&); W_SLOT(setValue_slot);

  void setValueType(qml_val_type::val_type valueType); W_SLOT(setValueType);
  void setAccess(qml_access_mode::access_mode access); W_SLOT(setAccess);
  void setBounding(qml_bounding_mode::bounding_mode bounding); W_SLOT(setBounding);
  void
  setFilterRepetitions(qml_rep_filter::repetition_filter filterRepetitions); W_SLOT(setFilterRepetitions);
  void setMin(QVariant min); W_SLOT(setMin);
  void setMax(QVariant max); W_SLOT(setMax);
  void setValues(QVariantList values); W_SLOT(setValues);
  void setUnit(QString unit); W_SLOT(setUnit);

  void resetValueType()
  {
    if (m_valueType)
    {
      m_valueType = ossia::none;
    }
  }; W_SLOT(resetValueType)
  void resetAccess()
  {
    if (m_access)
    {
      m_access = ossia::none;
    }
  }; W_SLOT(resetAccess)
  void resetBounding()
  {
    if (m_bounding)
    {
      m_bounding = ossia::none;
    }
  }; W_SLOT(resetBounding)
  void resetFilterRepetitions()
  {
    if (m_filterRepetitions)
    {
      m_filterRepetitions = ossia::none;
    }
  }; W_SLOT(resetFilterRepetitions)
  void resetMin()
  {
    setMin(QVariant{});
  }; W_SLOT(resetMin)
  void resetMax()
  {
    setMax(QVariant{});
  }; W_SLOT(resetMax)
  void resetValues()
  {
    if (m_values)
    {
      m_values = ossia::none;
    }
  }; W_SLOT(resetValues)
  void resetUnit()
  {
    if (m_unit)
    {
      m_values = ossia::none;
    }
  }; W_SLOT(resetUnit)

  void node_destroyed(); W_SLOT(node_destroyed);
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

W_PROPERTY(QVariantList, values READ values WRITE setValues NOTIFY valuesChanged RESET resetValues, W_Final)

W_PROPERTY(QVariant, max READ max WRITE setMax NOTIFY maxChanged RESET resetMax, W_Final)

W_PROPERTY(QVariant, min READ min WRITE setMin NOTIFY minChanged RESET resetMin, W_Final)

W_PROPERTY(ossia::qt::qml_rep_filter::repetition_filter, filterRepetitions READ filterRepetitions WRITE setFilterRepetitions NOTIFY filterRepetitionsChanged RESET resetFilterRepetitions, W_Final)

W_PROPERTY(ossia::qt::qml_bounding_mode::bounding_mode, bounding READ bounding WRITE setBounding NOTIFY boundingChanged RESET resetBounding, W_Final)

W_PROPERTY(ossia::qt::qml_access_mode::access_mode, access READ access WRITE setAccess NOTIFY accessChanged RESET resetAccess, W_Final)

W_PROPERTY(ossia::qt::qml_val_type::val_type, valueType READ valueType WRITE setValueType NOTIFY valueTypeChanged RESET resetValueType, W_Final)

W_PROPERTY(QString, unit READ unit WRITE setUnit NOTIFY unitChanged RESET resetUnit, W_Final)
};
}
}
