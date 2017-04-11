#include "qml_model_property.hpp"
#include <ossia-qt/device/qml_device.hpp>

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

}

void qml_model_property::setCount(int count)
{
  if (m_count == count)
    return;

  m_count = count;
  emit countChanged(count);
}

void qml_model_property::setDevice(QObject *device)
{
  auto olddev = m_device;
  auto newdev = qobject_cast<qml_device*>(device);
  if(olddev != newdev)
  {
    m_device = newdev;

    if(olddev)
    {
      olddev->models.erase(this);
    }

    if(newdev)
    {
      newdev->models.insert(this);
    }

    emit deviceChanged(device);
  }
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

}
}
