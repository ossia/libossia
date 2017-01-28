#include "qml_node_base.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia/network/base/node.hpp>

namespace ossia
{
namespace qt
{

qml_node_base::qml_node_base(QObject* parent):
  QObject{parent},
  m_device{&qml_singleton_device::instance()}
{

}

QString qml_node_base::node() const
{
  return m_node;
}

QObject*qml_node_base::device() const
{
  return m_device;
}

QString qml_node_base::path() const
{
  return m_path;
}

void qml_node_base::setNode(QString node)
{
  if (m_node == node)
    return;

  m_node = node;
  resetNode();
  emit nodeChanged(node);
}

void qml_node_base::setDevice(QObject* device)
{
  if (m_device == device)
    return;

  m_device = qobject_cast<qml_device*>(device);
  resetNode();
  emit deviceChanged(device);
}

ossia::net::node_base& qml_node_base::findClosestParent(
    QObject* obj,
    ossia::net::node_base& root)
{
  if(obj)
  {
    for(auto node : obj->children())
    {
      qml_node* casted = qobject_cast<qml_node*>(node);
      if(casted && casted->device() == device() && casted->ossiaNode())
        return *casted->ossiaNode();
    }

    if(auto par = obj->parent())
    {
      return findClosestParent(par, root);
    }
  }
  return root;
}

void qml_node_base::setPath(QString path)
{
  if (m_path == path)
    return;

  m_path = path;

  emit pathChanged(path);
}


}
}
