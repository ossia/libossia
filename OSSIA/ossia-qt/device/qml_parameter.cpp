// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qml_parameter.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <QDebug>
#include <QTimer>
#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::qt::qml_parameter)
namespace ossia
{
namespace qt
{
qml_parameter::qml_parameter(QQuickItem* parent) : qml_property_base(parent)
{
  connect(
      this, &qml_parameter::setValue_sig, this, &qml_parameter::setValue_slot,
      Qt::QueuedConnection);

  setDevice(&qml_singleton_device::instance());
  resetNode();
}

qml_parameter::~qml_parameter()
{
  if (m_ossia_node)
  {
    m_ossia_node->about_to_be_deleted
        .disconnect<qml_property_base, &qml_property_base::on_node_deleted>(
            this);
  }
  if (m_device)
    m_device->remove(this);
  m_param = nullptr;
}

void qml_parameter::setDevice(QObject* device)
{
  auto olddev = m_device;
  auto newdev = qobject_cast<qml_device*>(device);
  if (olddev != newdev)
  {
    if (olddev)
    {
      olddev->remove(this);
    }

    if (newdev)
    {
      newdev->add(this);
    }

    qml_node_base::setDevice(device);
  }
}

void qml_parameter::resetNode()
{
  const bool reading = m_device ? m_device->readPreset() : false;
  clearNode(reading);

  // Creation may not have finished yet.
  if (m_parentNode && !m_parentNode->ossiaNode())
  {
    setPath({});
    m_param = nullptr;
    m_callback = ossia::none;
    return;
  }

  if (m_device)
  {
    std::string node_name;
    bool relative = false;

    if (m_userRequestedNode.isEmpty())
    {
      node_name = "value";
      relative = true;
    }
    else if (m_userRequestedNode[0] != '/')
    {
      node_name = m_userRequestedNode.toStdString();
      relative = true;
    }
    else
    {
      node_name = m_userRequestedNode.toStdString();
    }

    m_ossia_node = ossia::net::find_or_create_node(
        get_parent(this->parentItem(), relative), node_name, reading);
    if (m_ossia_node)
    {
      m_ossia_node->about_to_be_deleted
          .connect<qml_property_base, &qml_property_base::on_node_deleted>(
              this);
      m_node = QString::fromStdString(m_ossia_node->get_name());
      nodeChanged(m_node);
      m_param = m_ossia_node->get_parameter();

      setPath(QString::fromStdString(
          ossia::net::address_string_from_node(*m_ossia_node)));
      setupAddress(reading);
      applyNodeAttributes();
      return;
    } // else, we go through the reset:
  }

  // In case something went wrong...
  setPath({});
  m_param = nullptr;
  m_callback = ossia::none;
}

qml_val_type::val_type qml_parameter::valueType() const
{
  if (m_param)
    return static_cast<qml_val_type::val_type>(m_param->get_value_type());
  return m_valueType ? *m_valueType : qml_val_type::val_type{};
}

qml_access_mode::access_mode qml_parameter::access() const
{
  if (m_param)
    return static_cast<qml_access_mode::access_mode>(m_param->get_access());
  return m_access ? *m_access : qml_access_mode::access_mode::Bi;
}

qml_bounding_mode::bounding_mode qml_parameter::bounding() const
{
  if (m_param)
    return static_cast<qml_bounding_mode::bounding_mode>(
        m_param->get_bounding());
  return m_bounding ? *m_bounding : qml_bounding_mode::bounding_mode::Free;
}

qml_rep_filter::repetition_filter qml_parameter::filterRepetitions() const
{
  if (m_param)
    return static_cast<qml_rep_filter::repetition_filter>(
        m_param->get_repetition_filter());
  return m_filterRepetitions ? *m_filterRepetitions
                             : qml_rep_filter::repetition_filter{};
}

QVariant qml_parameter::min() const
{
  if (m_param)
  {
    return m_param->get_domain().get_min().apply(ossia_to_qvariant{});
  }
  return m_min;
}

QVariant qml_parameter::max() const
{
  if (m_param)
  {
    return m_param->get_domain().get_max().apply(ossia_to_qvariant{});
  }
  return m_max;
}

QVariantList qml_parameter::values() const
{
  return *m_values;
}

QString qml_parameter::unit() const
{
  if (m_param)
    return QString::fromStdString(
        ossia::get_pretty_unit_text(m_param->get_unit()));
  return m_unit ? *m_unit : QString{};
}

void qml_parameter::setValue_slot(const ossia::value& v)
{
  auto next = ossia_to_qvariant{}(m_value.type(), v);
  if (m_value != next)
  {
    m_value = std::move(next);
    valueChanged(m_value);
  }
}

void qml_parameter::setValueType(qml_val_type::val_type valueType)
{
  if (m_valueType == valueType)
    return;

  m_valueType = valueType;
  if (m_param)
    m_param->set_value_type(static_cast<ossia::val_type>(valueType));
  valueTypeChanged(valueType);
}

void qml_parameter::setAccess(qml_access_mode::access_mode access)
{
  if (m_access == access)
    return;

  m_access = access;
  if (m_param)
    m_param->set_access(static_cast<ossia::access_mode>(access));
  accessChanged(access);
}

void qml_parameter::setBounding(qml_bounding_mode::bounding_mode bounding)
{
  if (m_bounding == bounding)
    return;

  m_bounding = bounding;
  if (m_param)
    m_param->set_bounding(static_cast<ossia::bounding_mode>(bounding));
  boundingChanged(bounding);
}

void qml_parameter::setFilterRepetitions(
    qml_rep_filter::repetition_filter filterRepetitions)
{
  if (m_filterRepetitions == filterRepetitions)
    return;

  m_filterRepetitions = filterRepetitions;
  if (m_param)
    m_param->set_repetition_filter(
        static_cast<ossia::repetition_filter>(filterRepetitions));
  filterRepetitionsChanged(filterRepetitions);
}

void qml_parameter::setMin(QVariant min)
{
  if (m_min == min)
    return;

  m_min = min;
  updateDomain();
  minChanged(min);
}

void qml_parameter::setMax(QVariant max)
{
  if (m_max == max)
    return;

  m_max = max;
  updateDomain();
  maxChanged(max);
}

void qml_parameter::setValues(QVariantList values)
{
  if (m_values == values)
    return;

  m_values = values;
  updateDomain();
  valuesChanged(values);
}

void qml_parameter::setUnit(QString unit)
{
  if (m_unit == unit)
    return;

  m_unit = unit;
  if (m_param)
    m_param->set_unit(ossia::parse_pretty_unit(unit.toStdString()));
  unitChanged(unit);
}

void qml_parameter::setValue(QVariant value)
{
  if (m_value == value)
    return;

  m_value = value;
  valueChanged(m_value);

  if (m_param)
  {
    if (m_callback)
      m_param->remove_callback(*m_callback);

    m_param->push_value(qt_to_ossia{}(m_value));

    m_callback = m_param->add_callback(
        [this](const ossia::value& v) { setValue_sig(v); });
  }
}

void qml_parameter::setupAddress(bool reading)
{
  if (reading)
  {
    if (m_param)
    {
      setValue_sig(m_param->value());
    }
    return;
  }

  m_param = nullptr;
  m_callback = ossia::none;
  if (m_ossia_node)
  {
    m_ossia_node->remove_parameter();

    m_param = m_ossia_node->create_parameter(
        m_valueType ? (ossia::val_type)*m_valueType
                    : ossia::val_type::IMPULSE);
    if (m_param)
    {
      if (!m_valueType)
        set_parameter_type(m_value.type(), *m_param);

      if (m_value.isValid())
        m_param->push_value(qt_to_ossia{}(m_value));

      if (m_access)
        m_param->set_access(static_cast<ossia::access_mode>(*m_access));

      if (m_bounding)
        m_param->set_bounding(
            static_cast<ossia::bounding_mode>(*m_bounding));

      if (m_filterRepetitions)
        m_param->set_repetition_filter(
            static_cast<ossia::repetition_filter>(*m_filterRepetitions));

      if (m_unit)
        m_param->set_unit(ossia::parse_pretty_unit(m_unit->toStdString()));

      updateDomain();
      m_callback = m_param->add_callback(
          [this](const ossia::value& v) { setValue_sig(v); });
      m_param->set_value_quiet(qt_to_ossia{}(m_value));
    }
  }
}

void qml_parameter::updateDomain()
{
  if(!m_param)
    return;
  auto val_min = qt_to_ossia{}(m_min);
  auto val_max = qt_to_ossia{}(m_max);
  if (m_values)
  {
    auto values = qt_to_ossia{}(*m_values);
    m_param->set_domain(ossia::make_domain(
        std::move(val_min), std::move(val_max), std::move(values)));
  }
  else
  {
    m_param->set_domain(
        ossia::make_domain(std::move(val_min), std::move(val_max)));
  }
}
}
}
