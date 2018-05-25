// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qml_property_reader.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/generic/generic_device.hpp>
#include <QDebug>

#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::qt::qml_property_context)
W_OBJECT_IMPL(ossia::qt::qml_property_reader)
W_OBJECT_IMPL(ossia::qt::qml_property_writer)
W_OBJECT_IMPL(ossia::qt::qml_binding)
W_OBJECT_IMPL(ossia::qt::qml_callback)
namespace ossia
{
namespace qt
{
qml_property_context::qml_property_context(
    QQmlProperty& p, net::parameter_base& addr, QObject* parent)
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
  m_param = nullptr;
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
          .connect<&qml_property_reader::on_node_deleted>(
              this);
      m_param = m_ossia_node->get_parameter();

      if (m_param)
      {
        m_propCtx
            = new qml_property_context{m_targetProperty, *m_param, this};
        m_param->push_value(qt_to_ossia{}(m_targetProperty.read()));
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
  nodeChanged(m_node);
  m_param = nullptr;
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
  m_param = nullptr;
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
          .connect<&qml_property_writer::on_node_deleted>(
              this);
      m_param = m_ossia_node->get_parameter();
      if (m_param)
      {
        m_cb = m_param->add_callback(
            [=](const ossia::value& v) { setValue_sig(v); });
        setValue_slot(m_param->value());
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
  nodeChanged(m_node);
  if (m_param)
    m_param->remove_callback(m_cb);
  m_cb = {};
  m_param = nullptr;
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
  m_param = nullptr;
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
          .connect<&qml_binding::on_node_deleted>(this);
      m_param = m_ossia_node->get_parameter();

      if (m_param)
      {
        m_expr = std::make_unique<QQmlExpression>(m_on);
        m_expr->setNotifyOnValueChanged(true);
        m_expr->evaluate(); // TODO QTBUG-60305

        connect(m_expr.get(), &QQmlExpression::valueChanged, this, [=]() {
          m_param->push_value(qt_to_ossia{}(m_expr->evaluate()));
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
  onChanged(on);
}

void qml_binding::on_node_deleted(const net::node_base&)
{
  clearNode();
}

void qml_binding::clearNode()
{
  m_node.clear();
  nodeChanged(m_node);
  m_param = nullptr;
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
  m_param = nullptr;
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
          .connect<&qml_callback::on_node_deleted>(this);
      m_param = m_ossia_node->get_parameter();
      if (m_param)
      {
        m_cb = m_param->add_callback(
            [=](const ossia::value& v) { setValue_sig(v); });
        setValue_slot(m_param->value());
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
  nodeChanged(m_node);
  if (m_param)
    m_param->remove_callback(m_cb);
  m_cb = {};
  m_param = nullptr;
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
  valueChanged(value);
}
}
}
