// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qml_model_property.hpp"
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia-qt/device/qml_device.hpp>

#include <boost/algorithm/string.hpp>
namespace ossia
{
namespace qt
{

qml_model_property::qml_model_property(QObject* parent)
    : QAbstractItemModel{parent}
{
  setDevice(&qml_singleton_device::instance());
}

qml_model_property::~qml_model_property()
{
  if(m_parentOssiaNode)
    m_parentOssiaNode->about_to_be_deleted.disconnect<qml_model_property, &qml_model_property::on_node_deleted>(*this);
  if (m_device)
  {
    m_device->remove(this);
  }
}

void qml_model_property::setCount(int count)
{
  if (m_count == count)
  {
    return;
  }

  beginResetModel();
  m_count = count;
  endResetModel();

  countChanged(count);
}

void qml_model_property::setDevice(QObject* device)
{
  auto olddev = m_device;
  auto newdev = qobject_cast<qml_device*>(device);
  if (olddev != newdev)
  {
    m_device = newdev;

    if (olddev)
    {
      olddev->remove(this);
      disconnect(
          olddev, &QObject::destroyed, this,
          &qml_model_property::on_device_deleted);
    }

    if (newdev)
    {
      newdev->add(this);
      connect(
          newdev, &QObject::destroyed, this,
          &qml_model_property::on_device_deleted);
    }

    deviceChanged(device);
  }
}

void qml_model_property::on_device_deleted(QObject*)
{
  m_device = nullptr;
  m_parentNode = nullptr;
}

void qml_model_property::setNode(QString node)
{
  if (m_node == node)
    return;

  m_node = node;
  nodeChanged(node);
}

void qml_model_property::reloadParentNode()
{
    if (m_parentOssiaNode)
    {
      m_parentOssiaNode->about_to_be_deleted.disconnect<qml_model_property, &qml_model_property::on_node_deleted>(*this);
    }

    if(m_parentNode)
    {
      auto pn = dynamic_cast<qml_node_base*>(m_parentNode);

      if(pn)
      {
        if(!pn->ossiaNode())
          pn->resetNode();
        m_parentOssiaNode = pn->ossiaNode();
      }
      else
      {
        auto dn = dynamic_cast<qml_device*>(m_parentNode);
        if(dn)
        {
          m_parentOssiaNode = &dn->device().get_root_node();
          setDevice(dn);
        }
        else
        {
          m_parentNode = &qml_singleton_device::instance();
          m_parentOssiaNode = &qml_singleton_device::instance().device().get_root_node();
        }
      }
    }
    else
    {
      m_parentNode = &qml_singleton_device::instance();
      m_parentOssiaNode = &qml_singleton_device::instance().device().get_root_node();
    }

    if (m_parentOssiaNode)
    {
      ossia::net::set_instance_bounds(
          *m_parentOssiaNode, ossia::net::instance_bounds{0, 1000});
      m_parentOssiaNode->about_to_be_deleted.connect<qml_model_property, &qml_model_property::on_node_deleted>(*this);
    }
}

void qml_model_property::setParentNode(QObject* parentNode)
{
  if (m_parentNode == parentNode)
    return;

  m_parentNode = parentNode;

  if (m_parentOssiaNode)
  {
    m_parentOssiaNode->about_to_be_deleted.disconnect<qml_model_property, &qml_model_property::on_node_deleted>(*this);
  }

  if(m_parentNode)
  {
    auto pn = dynamic_cast<qml_node_base*>(parentNode);

    if(pn)
    {
      if(!pn->ossiaNode())
        pn->resetNode();
      m_parentOssiaNode = pn->ossiaNode();
    }
    else
    {
      auto dn = dynamic_cast<qml_device*>(parentNode);
      if(dn)
      {
        m_parentOssiaNode = &dn->device().get_root_node();
        setDevice(dn);
      }
      else
      {
        m_parentNode = &qml_singleton_device::instance();
        m_parentOssiaNode = &qml_singleton_device::instance().device().get_root_node();
      }
    }
  }
  else
  {
    m_parentNode = &qml_singleton_device::instance();
    m_parentOssiaNode = &qml_singleton_device::instance().device().get_root_node();
  }

  if (m_parentOssiaNode)
  {
    ossia::net::set_instance_bounds(
        *m_parentOssiaNode, ossia::net::instance_bounds{0, 1000});
    m_parentOssiaNode->about_to_be_deleted.connect<qml_model_property, &qml_model_property::on_node_deleted>(*this);
  }

  parentNodeChanged(m_parentNode);
}

void qml_model_property::on_node_deleted(const net::node_base&)
{
  if(m_parentOssiaNode)
  {
    m_parentOssiaNode->about_to_be_deleted.disconnect<qml_model_property, &qml_model_property::on_node_deleted>(*this);
  }
  m_parentOssiaNode = nullptr;
}

QModelIndex
qml_model_property::index(int row, int column, const QModelIndex& parent) const
{
  return createIndex(row, column);
}

QModelIndex qml_model_property::parent(const QModelIndex& child) const
{
  return {};
}

int qml_model_property::rowCount(const QModelIndex& parent) const
{
  return m_count;
}

int qml_model_property::columnCount(const QModelIndex& parent) const
{
  return 0;
}

QVariant qml_model_property::data(const QModelIndex& index, int role) const
{
  return {};
}

int qml_model_property::count() const
{
  return m_count;
}

QString qml_model_property::node() const
{
  return m_node;
}

QObject* qml_model_property::parentNode() const
{
  return m_parentNode;
}

QObject* qml_model_property::device() const
{
  return m_device;
}
static bool is_instance(const std::string& root, const std::string& child)
{
  return boost::starts_with(child, root);
}

void qml_model_property::updateCount()
{
  // qDebug() << "updateCount: before" << m_parentNode << (void*)m_parentOssiaNode << m_node;
  reloadParentNode();
  // qDebug() << "updateCount: after" << m_parentNode << (void*)m_parentOssiaNode << m_node;
  int newCount = 0;

  if (m_parentOssiaNode && !m_node.isEmpty())
  {
    const std::string& instance_name = m_node.toStdString();

    // qDebug() << "updateCount: " << instance_name.c_str();
    for (auto& cld : m_parentOssiaNode->children())
    {
      const auto& name = cld->get_name();
      if (is_instance(instance_name, name))
        newCount++;
    }
  }
  else
  {
    // qDebug() << "updateCount: no node";
  }

  setCount(newCount);
}
}
}
