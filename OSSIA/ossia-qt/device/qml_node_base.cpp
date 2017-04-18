#include "qml_node_base.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/device/qml_node.hpp>
#include <ossia/network/base/node.hpp>
#include <QDebug>
#include <ossia-qt/js_utilities.hpp>
#include <QtQuick/QQuickItem>

namespace ossia
{
namespace qt
{

qml_node_base::qml_node_base(QQuickItem* parent):
  QQuickItem{parent}
{
}

qml_node_base::~qml_node_base()
{
  if(m_ossia_node)
  {
    auto par = m_ossia_node->getParent();
    if(par)
    {
      auto node = m_ossia_node;
      m_ossia_node = nullptr;
      par->removeChild(*node);
    }
  }
}

QString qml_node_base::node() const
{
  return m_node;
}

qml_device* qml_node_base::device() const
{
  return m_device;
}

net::node_base* qml_node_base::ossiaNode() { return m_ossia_node; }

QString qml_node_base::path() const
{
  return m_path;
}

qml_node_base*qml_node_base::parentNode() const
{
  return m_parentNode;
}

qint32 qml_node_base::priority() const
{
  if(m_ossia_node)
    if(auto prio = ossia::net::get_priority(*m_ossia_node))
      return *prio;
  return {};
}

QString qml_node_base::description() const
{
  if(m_ossia_node)
    if(auto desc = ossia::net::get_description(*m_ossia_node))
      return QString::fromStdString(*desc);
  return {};
}

static QStringList fromStringVector(const std::vector<std::string>& vec)
{
  QStringList l;
  l.reserve(vec.size());
  for(auto& s : vec)
    l.push_back(QString::fromStdString(s));
  return l;
}

static std::vector<std::string> toStringVector(const QStringList& l)
{
  std::vector<std::string> vec;
  vec.reserve(l.size());
  for(auto& s : l)
  {
    vec.push_back(s.toStdString());
  }
  return vec;
}

QStringList qml_node_base::tags() const
{
  if(m_ossia_node)
    if(auto tags = ossia::net::get_tags(*m_ossia_node))
      return fromStringVector(*tags);
  return m_tags;
}

qint32 qml_node_base::refreshRate() const
{
  if(m_ossia_node)
    if(auto rate = ossia::net::get_refresh_rate(*m_ossia_node))
      return *rate;
  return m_refreshRate;
}

double qml_node_base::stepSize() const
{
  if(m_ossia_node)
    if(auto val = ossia::net::get_value_step_size(*m_ossia_node))
      return *val;
  return m_stepSize;
}

QVariant qml_node_base::defaultValue() const
{
  if(m_ossia_node)
    if(auto dval = ossia::net::get_default_value(*m_ossia_node))
      return dval->apply(ossia_to_qvariant{});
  return m_defaultValue;
}

bool qml_node_base::critical() const
{
  if(m_ossia_node)
    return ossia::net::get_critical(*m_ossia_node);
  return m_critical;
}

bool qml_node_base::zombie() const
{
  if(m_ossia_node)
    return ossia::net::get_zombie(*m_ossia_node);
  return m_zombie;
}

void qml_node_base::setNode(QString node)
{
  m_node = node;
  m_userRequestedNode = node;
  resetNode();
  emit nodeChanged(node);
}

void qml_node_base::setDevice(qml_device* device)
{
  if (m_device == device)
    return;

  m_device = device;
  resetNode();
  emit deviceChanged(device);
}


void qml_node_base::setParentNode(qml_node_base* parentNode)
{
  if (m_parentNode == parentNode)
    return;

  m_parentNode = parentNode;
  if(m_parentNode)
    m_device = m_parentNode->device();
  resetNode();

  emit parentNodeChanged(parentNode);
}

void qml_node_base::setPriority(qint32 priority)
{
  if (m_priority == priority)
    return;

  m_priority = priority;

  if(m_ossia_node)
    ossia::net::set_priority(*m_ossia_node, m_priority);

  emit priorityChanged(priority);
}

void qml_node_base::setDescription(QString description)
{
  if (m_description == description)
    return;

  m_description = description;

  if(m_ossia_node)
    ossia::net::set_description(*m_ossia_node, m_description.toStdString());

  emit descriptionChanged(description);
}

void qml_node_base::setTags(QStringList tags)
{
  if (m_tags == tags)
    return;

  m_tags = tags;
  if(m_ossia_node)
    ossia::net::set_tags(*m_ossia_node, toStringVector(m_tags));

  emit tagsChanged(tags);
}

void qml_node_base::setRefreshRate(qint32 refreshRate)
{
  if (m_refreshRate == refreshRate)
    return;

  m_refreshRate = refreshRate;

  if(m_ossia_node)
    ossia::net::set_refresh_rate(*m_ossia_node, m_refreshRate);

  emit refreshRateChanged(refreshRate);
}

void qml_node_base::setStepSize(double stepSize)
{
  if (m_stepSize == stepSize)
    return;

  m_stepSize = stepSize;

  if(m_ossia_node)
    ossia::net::set_value_step_size(*m_ossia_node, stepSize);

  emit stepSizeChanged(stepSize);
}

void qml_node_base::setDefaultValue(QVariant defaultValue)
{
  if (m_defaultValue == defaultValue)
    return;

  m_defaultValue = defaultValue;

  if(m_ossia_node)
    ossia::net::set_default_value(*m_ossia_node, qt_to_ossia{}(m_defaultValue));

  emit defaultValueChanged(defaultValue);
}

void qml_node_base::setCritical(bool critical)
{
  if (m_critical == critical)
    return;

  m_critical = critical;

  if(m_ossia_node)
    ossia::net::set_critical(*m_ossia_node, m_critical);

  emit criticalChanged(critical);
}

void qml_node_base::applyNodeAttributes()
{
  return;
  if(m_ossia_node)
  {
    ossia::net::set_description(*m_ossia_node, m_description.toStdString());
    ossia::net::set_tags(*m_ossia_node, toStringVector(m_tags));
    ossia::net::set_priority(*m_ossia_node, m_priority);
    ossia::net::set_refresh_rate(*m_ossia_node, m_refreshRate);
    ossia::net::set_value_step_size(*m_ossia_node, m_stepSize);
    ossia::net::set_default_value(*m_ossia_node, qt_to_ossia{}(m_defaultValue));
    ossia::net::set_critical(*m_ossia_node, m_critical);
  }

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
      {
        return *casted->ossiaNode();
      }
    }

    auto item = qobject_cast<QQuickItem*>(obj);
    if(item)
    {
      for(auto child_item : item->childItems())
      {
        qml_node* casted = qobject_cast<qml_node*>(child_item);
        if(casted && casted->device() == device() && casted->ossiaNode())
        {
          return *casted->ossiaNode();
        }
      }

      if(auto par = item->parentItem())
      {
        return findClosestParent(par, root);
      }
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
