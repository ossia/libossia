#include "qml_model_property.hpp"
#include <ossia-qt/device/qml_device.hpp>

#include <boost/algorithm/string.hpp>
namespace ossia
{
namespace qt
{

qml_model_property::qml_model_property(QObject* parent):
  QAbstractItemModel{parent}
{
  setDevice(&qml_singleton_device::instance());
}


qml_model_property::~qml_model_property()
{
  if(m_device)
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

  emit countChanged(count);
}

void qml_model_property::setDevice(QObject* device)
{
  auto olddev = m_device;
  auto newdev = qobject_cast<qml_device*>(device);
  if(olddev != newdev)
  {
    m_device = newdev;

    if(olddev)
    {
      olddev->remove(this);
      disconnect(olddev, &QObject::destroyed, this, &qml_model_property::on_device_deleted);
    }

    if(newdev)
    {
      newdev->add(this);
      connect(newdev, &QObject::destroyed, this, &qml_model_property::on_device_deleted);
    }

    emit deviceChanged(device);
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
  emit nodeChanged(node);
}

void qml_model_property::setParentNode(qml_node_base* parentNode)
{
  if (m_parentNode == parentNode)
    return;

  m_parentNode = parentNode;
  if(m_parentNode && m_parentNode->ossiaNode())
  {
    ossia::net::set_instance_bounds(*m_parentNode->ossiaNode(), ossia::net::instance_bounds{0, 1000});
  }

  emit parentNodeChanged(parentNode);
}

void qml_model_property::on_node_deleted(const net::node_base&)
{
  m_parentNode = nullptr;
}

QModelIndex qml_model_property::index(int row, int column, const QModelIndex& parent) const
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

qml_node_base* qml_model_property::parentNode() const
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
  int newCount = 0;

  if(m_parentNode && !m_node.isEmpty())
  {
   if(auto on = m_parentNode->ossiaNode())
   {
     const std::string& instance_name = m_node.toStdString();
     for(auto& cld : on->children())
     {
       const auto& name = cld->getName();
       if(is_instance(instance_name, name))
         newCount++;
     }
   }
  }

  setCount(newCount);
}

}
}
