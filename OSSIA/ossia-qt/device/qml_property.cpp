#include "qml_device.hpp"
#include <ossia-qt/js_utilities.hpp>
#include "qml_property.hpp"
#include <iostream>
#include <QVector3D>
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

qml_property::qml_property(QObject *parent)
    : QObject(parent)
{
}

void qml_property::setTarget(const QQmlProperty &prop)
{
    m_targetProperty = prop;
    m_type = prop.propertyType();
    if(m_ossia_node)
        setupAddress();
}

QString qml_property::node() const
{
    return m_node;
}

QObject* qml_property::device() const
{
  return m_device;
}

void qml_property::setNode(QString node)
{
    if(!m_device)
        return;

    if (m_node == node)
        return;

    m_node = node;

    resetNode();
    emit nodeChanged(node);
}

void qml_property::setDevice(QObject* device)
{
  if (m_device == device)
    return;

  m_device = device;
  emit deviceChanged(device);
  resetNode();
}

void qml_property::resetNode()
{
  if(m_ossia_node)
  {
      auto par = m_ossia_node->getParent();
      if(par)
      {
        par->removeChild(*m_ossia_node);
      }
  }

  if(auto dev = dynamic_cast<ossia::qt::qml_device*>(m_device))
  {
    m_ossia_node = &ossia::net::find_or_create_node(
                dev->device().getRootNode(),
                m_node.toStdString());

    setupAddress();
  }
}

void qml_property::setupAddress()
{
  m_ossia_node->removeAddress();
  m_address = m_ossia_node->createAddress(ossia::val_type::IMPULSE);
  set_address_type((QVariant::Type)m_targetProperty.propertyType(), *m_address);

}

}
}





