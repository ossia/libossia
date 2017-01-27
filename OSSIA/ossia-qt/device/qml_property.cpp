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

void qml_node::resetNode()
{
  if(m_ossia_node)
  {
    auto par = m_ossia_node->getParent();
    if(par)
    {
      par->removeChild(*m_ossia_node);
    }
  }

  if(auto dev = qobject_cast<ossia::qt::qml_device*>(m_device))
  {
    std::string node_name;
    bool relative = false;

    if(m_node.isEmpty())
    {
      if(auto par = this->parent())
      {
        node_name = par->objectName().toStdString();
        if(node_name.empty())
          node_name = par->metaObject()->className();
      }

      if(node_name.empty())
        node_name = "Object";

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
        ? findClosestParent(this->parent(), dev->device().getRootNode())
        : dev->device().getRootNode();

    auto addr = &dev->device().getRootNode();
    qDebug() << addr;
    m_ossia_node = &ossia::net::find_or_create_node(parent, node_name);
    setPath(
          QString::fromStdString(
            ossia::net::address_string_from_node(*m_ossia_node)));

    reparentChildren();
    return;
  }
  reparentChildren();

  // In case something went wrong...
  setPath({});
}

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
}

void qml_property::setTarget(const QQmlProperty &prop)
{
  disconnect(m_conn);
  m_targetProperty = prop;
  resetNode();
}

static tsl::hopscotch_map<QVariant::Type, QMetaMethod>& qml_methods()
{
  static const auto method_offset = qml_property::staticMetaObject.indexOfMethod("qtBoolValueChanged(bool)");
  static tsl::hopscotch_map<QVariant::Type, QMetaMethod> m{
  {QVariant::Bool, qml_property::staticMetaObject.method(method_offset + 0)},
  {QVariant::Time, qml_property::staticMetaObject.method(method_offset + 1)},
  {QVariant::Int, qml_property::staticMetaObject.method(method_offset + 2)},
  {QVariant::UInt, qml_property::staticMetaObject.method(method_offset + 3)},
  {QVariant::ULongLong, qml_property::staticMetaObject.method(method_offset + 4)},
  {QVariant::Char, qml_property::staticMetaObject.method(method_offset + 5)},
  {QVariant::String, qml_property::staticMetaObject.method(method_offset + 6)},
  {QVariant::ByteArray, qml_property::staticMetaObject.method(method_offset + 7)},
  {QVariant::Double, qml_property::staticMetaObject.method(method_offset + 8)},
  {QVariant::Color, qml_property::staticMetaObject.method(method_offset + 9)},
  {QVariant::Point, qml_property::staticMetaObject.method(method_offset + 10)},
  {QVariant::PointF, qml_property::staticMetaObject.method(method_offset + 11)},
  {QVariant::Vector2D, qml_property::staticMetaObject.method(method_offset + 12)},
  {QVariant::Vector3D, qml_property::staticMetaObject.method(method_offset + 13)},
  {QVariant::Vector4D, qml_property::staticMetaObject.method(method_offset + 14)},
  {QVariant::Quaternion, qml_property::staticMetaObject.method(method_offset + 15)},
  {QVariant::Line, qml_property::staticMetaObject.method(method_offset + 16)},
  {QVariant::LineF, qml_property::staticMetaObject.method(method_offset + 17)},
  {QVariant::Rect, qml_property::staticMetaObject.method(method_offset + 18)},
  {QVariant::RectF, qml_property::staticMetaObject.method(method_offset + 19)},
  {QVariant::Size, qml_property::staticMetaObject.method(method_offset + 20)},
  {QVariant::SizeF, qml_property::staticMetaObject.method(method_offset + 21)},
  {QVariant::List, qml_property::staticMetaObject.method(method_offset + 22)},
  {QVariant::StringList, qml_property::staticMetaObject.method(method_offset + 23)},
  {QVariant::Date, qml_property::staticMetaObject.method(method_offset + 24)},
  {QVariant::Invalid, qml_property::staticMetaObject.method(method_offset + 25)}, // impulse
  {QVariant::UserType, qml_property::staticMetaObject.method(method_offset + 26)} // variant
  };

  return m;
}


void qml_property::setupAddress()
{
  static const auto idx = qml_property::staticMetaObject.indexOfMethod("qtVariantChanged()");

  m_address = nullptr;
  if(m_ossia_node)
  {
    m_ossia_node->removeAddress();
    m_address = m_ossia_node->createAddress(ossia::val_type::IMPULSE);
    set_address_type((QVariant::Type)m_targetProperty.propertyType(), *m_address);
    m_address->setValueQuiet(qt_to_ossia{}(m_targetProperty.read()));
    if(m_targetProperty.hasNotifySignal())
    {
      qDebug() << "connection"
               << m_targetProperty.name()
               << m_targetProperty.propertyType()
               << m_targetProperty.method().methodSignature()
               << staticMetaObject.method(idx).methodSignature();
      m_targetProperty.connectNotifySignal(this, SLOT(qtVariantChanged()));
    }

    m_address->setValueQuiet(qt_to_ossia{}(m_targetProperty.read()));
  }
}

}
}
