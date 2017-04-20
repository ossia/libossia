#include "qml_property.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <QDebug>

namespace ossia
{
namespace qt
{
void qml_property::resetNode()
{
  const bool reading = m_device ? m_device->readPreset() : false;
  clearNode(reading);

  // Creation may not have finished yet.
  if(m_parentNode && !m_parentNode->ossiaNode())
  {
    setPath({});
    m_address = nullptr;
    return;
  }

  if(m_device)
  {
    std::string node_name;
    bool relative = false;

    if(m_userRequestedNode.isEmpty())
    {
      node_name = m_targetProperty.name().replace('.', '_').toStdString();
      relative = true;
    }
    else if(m_userRequestedNode[0] != '/')
    {
      node_name = m_userRequestedNode.toStdString();
      relative = true;
    }
    else
    {
      node_name = m_userRequestedNode.toStdString();
    }

    auto get_parent = [&] () -> ossia::net::node_base&
    {
      if(m_parentNode)
        return *m_parentNode->ossiaNode();

      if(relative)
      {
        return findClosestParent(m_targetProperty.object(), m_device->device().getRootNode());
      }
      else
      {
        return m_device->device().getRootNode();
      }
    };

    ossia::net::node_base& parent = get_parent();

    if(reading)
    {
      m_ossia_node = ossia::net::find_node(parent, node_name);
    }
    else
    {
      m_ossia_node = &ossia::net::create_node(parent, node_name);
    }

    if(m_ossia_node)
    {
      m_ossia_node->aboutToBeDeleted.connect<qml_property, &qml_property::on_node_deleted>(this);
      m_node = QString::fromStdString(m_ossia_node->getName());
      m_address = m_ossia_node->getAddress();
      applyNodeAttributes();

      setPath(
            QString::fromStdString(
              ossia::net::address_string_from_node(*m_ossia_node)));
      setupAddress(reading);
      return;
    } // else, we go through the reset:
  }

  // In case something went wrong...
  setPath({});
  m_address = nullptr;
}

void qml_property::remapNode()
{
  clearNode(false);

  if(m_device && m_userRequestedNode.startsWith('/'))
  {
    m_ossia_node = ossia::net::find_node(
                     m_device->device().getRootNode(),
                     m_userRequestedNode.toStdString());
    if(m_ossia_node)
    {
      m_address = m_ossia_node->getAddress();
      if(m_targetProperty.hasNotifySignal())
      {
        m_targetProperty.connectNotifySignal(this, SLOT(qtVariantChanged()));
      }

      m_address->add_callback([this] (const ossia::value& v) { setValue_sig(v); });
      m_address->setValueQuiet(qt_to_ossia{}(m_targetProperty.read()));
    }
  }
}

qml_property::qml_property(QQuickItem *parent)
  : qml_node_base(parent)
{
  connect(this, &qml_property::setValue_sig,
          this, &qml_property::setValue_slot,
          Qt::QueuedConnection);

  setDevice(&qml_singleton_device::instance());
  resetNode();
}

qml_property::~qml_property()
{
  if(m_device) m_device->properties.erase(this);
  m_address = nullptr;
}

void qml_property::setTarget(const QQmlProperty &prop)
{
  m_targetProperty = prop;
  resetNode();
}

void qml_property::qtVariantChanged()
{
  if(m_address)
  {
    m_address->setValueQuiet(qt_to_ossia{}(m_targetProperty.read()));
    m_device->device().getProtocol().push(*m_address);
  }
}

void qml_property::setDevice(QObject* device)
{
  auto olddev = m_device;
  auto newdev = qobject_cast<qml_device*>(device);
  if(olddev != newdev)
  {
    if(olddev)
    {
      olddev->properties.erase(this);
    }

    if(newdev)
    {
      newdev->properties.insert({this, this});
    }

    qml_node_base::setDevice(device);
  }
}

void qml_property::updateQtValue()
{
  setValue_sig(m_address->cloneValue());
}

qml_context::val_type qml_property::valueType() const
{
  if(m_address)
    return static_cast<qml_context::val_type>(m_address->getValueType());
  return m_valueType;
}

qml_context::access_mode qml_property::access() const
{
  if(m_address)
    return static_cast<qml_context::access_mode>(m_address->getAccessMode());
  return m_access;
}

qml_context::bounding_mode qml_property::bounding() const
{
  if(m_address)
    return static_cast<qml_context::bounding_mode>(m_address->getBoundingMode());
  return m_bounding;
}

qml_context::repetition_filter qml_property::filterRepetitions() const
{
  if(m_address)
    return static_cast<qml_context::repetition_filter>(m_address->getRepetitionFilter());
  return m_filterRepetitions;
}

QVariant qml_property::min() const
{
  return m_min;
}

QVariant qml_property::max() const
{
  return m_max;
}

QVariantList qml_property::values() const
{
  return m_values;
}

QString qml_property::unit() const
{
  return m_unit;
}

void qml_property::setValue_slot(const value& v)
{
  auto cur = m_targetProperty.read();
  auto next = ossia_to_qvariant{}((QVariant::Type)m_targetProperty.propertyType(), v);
  if(cur != next)
    m_targetProperty.write(next);

}

void qml_property::setValueType(qml_context::val_type valueType)
{
  if (m_valueType == valueType)
    return;

  m_valueType = valueType;
  if(m_address)
    m_address->setValueType(static_cast<ossia::val_type>(m_valueType));
  emit valueTypeChanged(valueType);
}

void qml_property::setAccess(qml_context::access_mode access)
{
  if (m_access == access)
    return;

  m_access = access;
  if(m_address)
    m_address->setAccessMode(static_cast<ossia::access_mode>(m_access));
  emit accessChanged(access);
}

void qml_property::setBounding(qml_context::bounding_mode bounding)
{
  if (m_bounding == bounding)
    return;

  m_bounding = bounding;
  if(m_address)
    m_address->setBoundingMode(static_cast<ossia::bounding_mode>(m_bounding));
  emit boundingChanged(bounding);
}

void qml_property::setFilterRepetitions(qml_context::repetition_filter filterRepetitions)
{
  if (m_filterRepetitions == filterRepetitions)
    return;

  m_filterRepetitions = filterRepetitions;
  if(m_address)
    m_address->setRepetitionFilter(static_cast<ossia::repetition_filter>(m_filterRepetitions));
  emit filterRepetitionsChanged(filterRepetitions);
}

void qml_property::setMin(QVariant min)
{
  if (m_min == min)
    return;

  m_min = min;
  updateDomain();
  emit minChanged(min);
}

void qml_property::setMax(QVariant max)
{
  if (m_max == max)
    return;

  m_max = max;
  updateDomain();
  emit maxChanged(max);
}

void qml_property::setValues(QVariantList values)
{
  if (m_values == values)
    return;

  m_values = values;
  updateDomain();
  emit valuesChanged(values);
}

void qml_property::setUnit(QString unit)
{
  if (m_unit == unit)
    return;

  m_unit = unit;
  if(m_address)
    m_address->setUnit(ossia::parse_pretty_unit(unit.toStdString()));
  emit unitChanged(unit);
}

void qml_property::setupAddress(bool reading)
{
  if(reading)
  {
    if(m_address)
    {
      updateQtValue();
    }
    return;
  }

  m_address = nullptr;
  if(m_ossia_node)
  {
    m_ossia_node->removeAddress();
    m_address = m_ossia_node->createAddress(ossia::val_type::IMPULSE);
    if(m_address)
    {
      set_address_type((QVariant::Type)m_targetProperty.propertyType(), *m_address);

      if(m_targetProperty.hasNotifySignal())
      {
        m_targetProperty.connectNotifySignal(this, SLOT(qtVariantChanged()));
      }

      m_address->add_callback([this] (const ossia::value& v) { setValue_sig(v); });
      m_address->setValueQuiet(qt_to_ossia{}(m_targetProperty.read()));
      /* TODO set them all as optional<> and only update them if they are set.
      m_address->setValueType(static_cast<ossia::val_type>(m_valueType));
      m_address->setAccessMode(static_cast<ossia::access_mode>(m_access));
      m_address->setBoundingMode(static_cast<ossia::bounding_mode>(m_bounding));
      m_address->setRepetitionFilter(static_cast<ossia::repetition_filter>(m_filterRepetitions));
      m_address->setUnit(ossia::parse_pretty_unit(m_unit.toStdString()));
      updateDomain();
      */
    }
  }
}

void qml_property::updateDomain()
{
  auto val_min = qt_to_ossia{}(m_min);
  auto val_max = qt_to_ossia{}(m_max);
  auto values = qt_to_ossia{}(m_values);
  m_address->setDomain(ossia::make_domain(val_min, val_max, values));
}

void qml_property::on_node_deleted(const net::node_base&)
{
  m_address = nullptr;
  m_ossia_node = nullptr;
}

void qml_property::clearNode(bool reading)
{
  m_node.clear();
  if(m_ossia_node)
  {
    auto par = m_ossia_node->getParent();
    if(par)
    {
      auto node = m_ossia_node;
      m_address = nullptr;
      m_ossia_node = nullptr;
      if(!reading)
        par->removeChild(*node);
    }
  }
}

}
}
