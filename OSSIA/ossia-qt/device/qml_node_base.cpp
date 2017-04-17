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

QVariantMap qml_node_base::extended() const
{
  QVariantMap map;

  if(m_ossia_node)
  {
    ossia::extended_attributes& xt = *m_ossia_node;
    // First remove all the existing keys
    for(const auto& pair : xt)
    {
      map.insert(QString::fromStdString(pair.first), anyToVariant(pair.second));
    }
  }

  return map;
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

QStringList qml_node_base::tags() const
{
  if(m_ossia_node)
    if(auto tags = ossia::net::get_tags(*m_ossia_node))
      return fromStringVector(*tags);
  return {};
}

qint32 qml_node_base::refreshRate() const
{
  if(m_ossia_node)
    if(auto rate = ossia::net::get_refresh_rate(*m_ossia_node))
      return *rate;
  return {};
}

double qml_node_base::stepSize() const
{
  if(m_ossia_node)
    if(auto val = ossia::net::get_value_step_size(*m_ossia_node))
      return *val;
  return {};
}

QVariant qml_node_base::defaultValue() const
{
  if(m_ossia_node)
    if(auto dval = ossia::net::get_default_value(*m_ossia_node))
      return dval->apply(ossia_to_qvariant{});
  return {};
}

bool qml_node_base::critical() const
{
  if(m_ossia_node)
    return ossia::net::get_critical(*m_ossia_node);
  return {};
}

bool qml_node_base::zombie() const
{
  if(m_ossia_node)
    return ossia::net::get_zombie(*m_ossia_node);
  return {};
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

void qml_node_base::setExtended(QVariantMap extended)
{
  auto current = this->extended();

  if (current == extended)
    return;

  if(m_ossia_node)
  {
    ossia::extended_attributes& xt = *m_ossia_node;

    // First remove all the erased keys
    auto cur_end = current.cend();
    for(auto it = current.cbegin(); it != cur_end; ++it)
    {
      const auto& k = it.key();
      if(!extended.contains(k))
        xt.erase(k.toStdString());
    }

    // Then insert the new ones
    auto ext_end = extended.end();
    for(auto it = extended.begin(); it != ext_end; ++it)
    {
      xt[it.key().toStdString()] = anyToVariant(it.value());
    }
  }
  emit extendedChanged(extended);
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
  emit priorityChanged(priority);
}

void qml_node_base::setDescription(QString description)
{
  if (m_description == description)
    return;

  m_description = description;
  emit descriptionChanged(description);
}

void qml_node_base::setTags(QStringList tags)
{
  if (m_tags == tags)
    return;

  m_tags = tags;
  emit tagsChanged(tags);
}

void qml_node_base::setRefreshRate(qint32 refreshRate)
{
  if (m_refreshRate == refreshRate)
    return;

  m_refreshRate = refreshRate;
  emit refreshRateChanged(refreshRate);
}

void qml_node_base::setStepSize(double stepSize)
{
  if (m_stepSize == stepSize)
    return;

  m_stepSize = stepSize;
  emit stepSizeChanged(stepSize);
}

void qml_node_base::setDefaultValue(QVariant defaultValue)
{
  if (m_defaultValue == defaultValue)
    return;

  m_defaultValue = defaultValue;
  emit defaultValueChanged(defaultValue);
}

void qml_node_base::setCritical(bool critical)
{
  if (m_critical == critical)
    return;

  m_critical = critical;
  emit criticalChanged(critical);
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

QVariant qml_node_base::anyToVariant(const boost::any& any) const
{
  if(auto v = boost::any_cast<QVariant>(&any))
    return *v;
  else if(auto s = boost::any_cast<std::string>(&any))
    return QString::fromStdString(*s);
  else if(auto vec = boost::any_cast<std::vector<std::string>>(&any))
  {
    QStringList s;
    s.reserve(vec->size());
    for(auto& string : *vec)
      s.append(QString::fromStdString(string));
    return s;
  }
  else if(auto i = boost::any_cast<int>(&any))
    return *i;
  else if(auto f = boost::any_cast<float>(&any))
    return *f;
  else if(auto d = boost::any_cast<double>(&any))
    return *d;
  else if(auto b = boost::any_cast<bool>(&any))
    return *b;
  return {};

}

}
}
