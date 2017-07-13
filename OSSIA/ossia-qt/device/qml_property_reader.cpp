#include "qml_property_reader.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <QDebug>

namespace ossia
{
namespace qt
{
qml_property_context::qml_property_context(
    QQmlProperty& p, net::address_base& addr, QObject* parent)
    : QObject{parent}, targetProperty{p}, address{addr}
{
  if (targetProperty.hasNotifySignal())
  {
    targetProperty.connectNotifySignal(this, SLOT(qtVariantChanged()));
  }
}

void qml_property_context::qtVariantChanged()
{
  address.push_value(qt_to_ossia{}(targetProperty.read()));
}

qml_property_reader::qml_property_reader(QQuickItem* parent)
    : qml_node_base(parent)
{
  setDevice(&qml_singleton_device::instance());
  resetNode();
}

qml_property_reader::~qml_property_reader()
{
  if (m_device)
    m_device->remove(this);
  m_address = nullptr;
}

void qml_property_reader::setTarget(const QQmlProperty& prop)
{
  m_targetProperty = prop;
  resetNode();
}

void qml_property_reader::setDevice(QObject* device)
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

void qml_property_reader::resetNode()
{
  clearNode();

  if (m_device && m_userRequestedNode.startsWith('/'))
  {
    m_ossia_node = ossia::net::find_node(
        m_device->device().get_root_node(), m_userRequestedNode.toStdString());

    if (m_ossia_node)
    {
      m_ossia_node->about_to_be_deleted
          .connect<qml_property_reader, &qml_property_reader::on_node_deleted>(
              this);
      m_address = m_ossia_node->get_address();

      if (m_address)
      {
        m_propCtx
            = new qml_property_context{m_targetProperty, *m_address, this};
        m_address->push_value(qt_to_ossia{}(m_targetProperty.read()));
      }
    }
  }
}

void qml_property_reader::on_node_deleted(const net::node_base&)
{
  clearNode();
}

void qml_property_reader::clearNode()
{
  m_node.clear();
  m_address = nullptr;
  m_ossia_node = nullptr;

  if (m_propCtx)
    delete m_propCtx;
  m_propCtx = nullptr;
}

qml_property_writer::qml_property_writer(QQuickItem* parent)
    : qml_node_base(parent)
{
  connect(
      this, &qml_property_writer::setValue_sig, this,
      &qml_property_writer::setValue_slot, Qt::QueuedConnection);

  setDevice(&qml_singleton_device::instance());
  resetNode();
}

qml_property_writer::~qml_property_writer()
{
  if (m_device)
    m_device->remove(this);
  m_address = nullptr;
}

void qml_property_writer::setTarget(const QQmlProperty& prop)
{
  m_targetProperty = prop;
  resetNode();
}

void qml_property_writer::setDevice(QObject* device)
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

void qml_property_writer::resetNode()
{
  clearNode();

  if (m_device && m_userRequestedNode.startsWith('/'))
  {
    m_ossia_node = ossia::net::find_node(
        m_device->device().get_root_node(), m_userRequestedNode.toStdString());

    if (m_ossia_node)
    {
      m_ossia_node->about_to_be_deleted
          .connect<qml_property_writer, &qml_property_writer::on_node_deleted>(
              this);
      m_address = m_ossia_node->get_address();
      if (m_address)
      {
        m_cb = m_address->add_callback(
            [=](const ossia::value& v) { setValue_sig(v); });
        setValue_slot(m_address->value());
      }
    }
  }
}

void qml_property_writer::on_node_deleted(const net::node_base&)
{
  clearNode();
}

void qml_property_writer::clearNode()
{
  m_node.clear();
  if (m_address)
    m_address->remove_callback(m_cb);
  m_cb = {};
  m_address = nullptr;
  m_ossia_node = nullptr;
}

void qml_property_writer::setValue_slot(const value& v)
{
  auto cur = m_targetProperty.read();
  auto next = ossia_to_qvariant{}(
      (QVariant::Type)m_targetProperty.propertyType(), v);
  if (cur != next)
    m_targetProperty.write(next);
}

qml_binding::qml_binding(QQuickItem* parent) : qml_node_base(parent)
{
  setDevice(&qml_singleton_device::instance());
  resetNode();
}

qml_binding::~qml_binding()
{
  if (m_device)
    m_device->remove(this);
  m_address = nullptr;
}

void qml_binding::setDevice(QObject* device)
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

void qml_binding::resetNode()
{
  clearNode();

  if (m_device && m_userRequestedNode.startsWith('/'))
  {
    m_ossia_node = ossia::net::find_node(
        m_device->device().get_root_node(), m_userRequestedNode.toStdString());

    if (m_ossia_node)
    {
      m_ossia_node->about_to_be_deleted
          .connect<qml_binding, &qml_binding::on_node_deleted>(this);
      m_address = m_ossia_node->get_address();

      if (m_address)
      {
        m_expr = std::make_unique<QQmlExpression>(m_on);
        m_expr->setNotifyOnValueChanged(true);
        m_expr->evaluate(); // TODO QTBUG-60305

        connect(m_expr.get(), &QQmlExpression::valueChanged, this, [=]() {
          m_address->push_value(qt_to_ossia{}(m_expr->evaluate()));
        });
      }
    }
  }
}

QQmlScriptString qml_binding::on() const
{
  return m_on;
}

void qml_binding::setOn(QQmlScriptString on)
{
  if (m_on == on)
    return;

  m_on = on;
  emit onChanged(on);
}

void qml_binding::on_node_deleted(const net::node_base&)
{
  clearNode();
}

void qml_binding::clearNode()
{
  m_node.clear();
  m_address = nullptr;
  m_ossia_node = nullptr;
  m_expr.reset();
}

qml_callback::qml_callback(QQuickItem* parent) : qml_node_base(parent)
{
  connect(
      this, &qml_callback::setValue_sig, this, &qml_callback::setValue_slot,
      Qt::QueuedConnection);

  setDevice(&qml_singleton_device::instance());
  resetNode();
}

qml_callback::~qml_callback()
{
  if (m_device)
    m_device->remove(this);
  m_address = nullptr;
}

void qml_callback::setDevice(QObject* device)
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

void qml_callback::resetNode()
{
  clearNode();

  if (m_device && m_userRequestedNode.startsWith('/'))
  {
    m_ossia_node = ossia::net::find_node(
        m_device->device().get_root_node(), m_userRequestedNode.toStdString());

    if (m_ossia_node)
    {
      m_ossia_node->about_to_be_deleted
          .connect<qml_callback, &qml_callback::on_node_deleted>(this);
      m_address = m_ossia_node->get_address();
      if (m_address)
      {
        m_cb = m_address->add_callback(
            [=](const ossia::value& v) { setValue_sig(v); });
        setValue_slot(m_address->value());
      }
    }
  }
}

void qml_callback::on_node_deleted(const net::node_base&)
{
  clearNode();
}

void qml_callback::clearNode()
{
  m_node.clear();
  if (m_address)
    m_address->remove_callback(m_cb);
  m_cb = {};
  m_address = nullptr;
  m_ossia_node = nullptr;
}

void qml_callback::setValue_slot(const ossia::value& v)
{
  setValue(v.apply(ossia_to_qvariant{}));
}

QVariant qml_callback::value() const
{
  return m_value;
}

void qml_callback::setValue(QVariant value)
{
  if (m_value == value)
    return;

  m_value = value;
  emit valueChanged(value);
}
}
}
