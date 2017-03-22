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

static void reparent_rec(QQuickItem* it)
{
  if(auto obj = qobject_cast<qml_node_base*>(it))
    obj->resetNode();

  for(auto cld : it->childItems())
  {
    if(cld != it)
    {
      reparent_rec(it);
    }
  }
}

void qml_node_base::reparentChildren()
{
  if(auto par = parentItem())
  {
    const auto& children = par->childItems();
    for(auto cld : children)
    {
      if(cld != this)
      {
        reparent_rec(cld);
      }
    }
  }

  if(auto par = parent())
  {
    for(auto cld : par->findChildren<qml_node_base*>())
    {
      if(cld != this)
      {
        cld->resetNode();
      }
    }
  }
}


void qml_property::resetNode(bool recursive)
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
      par->removeChild(*node);
    }
  }

  if(auto dev = qobject_cast<ossia::qt::qml_device*>(m_device))
  {
    std::string node_name;
    bool relative = false;

    if(m_userRequestedNode.isEmpty())
    {
      node_name = m_targetProperty.name().toStdString();
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

    ossia::net::node_base& parent =
        relative
        ? findClosestParent(m_targetProperty.object(), dev->device().getRootNode())
        : dev->device().getRootNode();

    m_ossia_node = &ossia::net::create_node(parent, node_name);

    m_ossia_node->aboutToBeDeleted.connect<qml_property, &qml_property::on_node_deleted>(this);
    m_node = QString::fromStdString(m_ossia_node->getName());

    setPath(
          QString::fromStdString(
            ossia::net::address_string_from_node(*m_ossia_node)));
    setupAddress();
  }

  // In case something went wrong...
  setPath({});
  m_address = nullptr;
}

qml_property::qml_property(QQuickItem *parent)
  : qml_node_base(parent)
{
  connect(this, &qml_property::setValue_sig,
          this, &qml_property::setValue_slot,
          Qt::QueuedConnection);

  resetNode();
}

qml_property::~qml_property()
{
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

void qml_property::setValue_slot(const value& v)
{
  auto cur = m_targetProperty.read();
  auto next = ossia_to_qvariant{}((QVariant::Type)m_targetProperty.propertyType(), v);
  if(cur != next)
    m_targetProperty.write(next);
}

void qml_property::setupAddress()
{
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
    }
  }
}

void qml_property::on_node_deleted(const net::node_base&)
{
  m_address = nullptr;
  m_ossia_node = nullptr;
}

}
}
