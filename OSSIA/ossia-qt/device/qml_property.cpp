#include "qml_property.hpp"
#include <ossia-qt/device/qml_device.hpp>
#include <ossia-qt/js_utilities.hpp>

#include <ossia/network/generic/generic_device.hpp>

// En plus de ce qu'il y a, on veut :
// - par défaut chercher un device global, sinon on peut lui affecter un device:
// - noeuds : pouvoir donner une adresse complète ou bien juste un nom.
//   Par défaut prend le propertyName / objectName / className ?
//   Et remonter récursivement jusqu'à trouver un objet qui a une propriété "node".

namespace ossia
{
namespace qt
{

void qml_node_base::reparentChildren()
{
  for(auto cld : this->parent()->findChildren<qml_node_base*>())
  {
    if(cld != this)
    {
      cld->resetNode();
    }
  }
}


void qml_property::resetNode()
{
  if(m_ossia_node)
  {
    auto par = m_ossia_node->getParent();
    if(par)
    {
      par->removeChild(*m_ossia_node);
      m_ossia_node = nullptr;
      m_address = nullptr;
    }
  }

  if(auto dev = qobject_cast<ossia::qt::qml_device*>(m_device))
  {
    std::string node_name;
    bool relative = false;

    if(m_node.isEmpty())
    {
      node_name = m_targetProperty.name().toStdString();
      relative = true;
    }
    else if(m_node[0] != '/')
    {
      relative = true;
      node_name = m_node.toStdString();
    }
    else
    {
      node_name = m_node.toStdString();
    }

    ossia::net::node_base& parent =
        relative
        ? findClosestParent(m_targetProperty.object(), dev->device().getRootNode())
        : dev->device().getRootNode();

    m_ossia_node = &ossia::net::find_or_create_node(parent, node_name);
    setPath(
          QString::fromStdString(
            ossia::net::address_string_from_node(*m_ossia_node)));
    setupAddress();
    return;
  }

  // In case something went wrong...
  setPath({});
  m_address = nullptr;
}

qml_property::qml_property(QObject *parent)
  : qml_node_base(parent)
{
  connect(this, &qml_property::setValue_sig,
          this, &qml_property::setValue_slot,
          Qt::QueuedConnection);
}

qml_property::~qml_property()
{
  m_address = nullptr;
}

void qml_property::setTarget(const QQmlProperty &prop)
{
  disconnect(m_conn);
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

}
}
