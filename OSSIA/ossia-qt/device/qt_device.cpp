#include "qt_device.hpp"
#include <QEvent>
#include <QDebug>

namespace ossia
{
namespace qt
{

qt_object_node::qt_object_node(QObject& obj, net::device_base& device):
  generic_node_base{{}, device}
{
  init(obj);
}

qt_object_node::qt_object_node(QObject& obj, net::device_base& device, net::node_base& aParent):
  generic_node_base{{}, device, aParent}
{
  init(obj);
}

net::address_base* qt_object_node::getAddress() const
{
  return mAddress.get();
}

net::address_base* qt_object_node::createAddress(val_type type)
{
  return nullptr;
}

bool qt_object_node::removeAddress()
{
  return false;
}

void qt_object_node::init(QObject& obj)
{
  setName(obj.objectName().toStdString());

  for(auto c : obj.children())
  {
    mChildren.push_back(
          std::make_unique<qt_object_node>(*c, mDevice, *this));
  }

  for(int i = 0; i < obj.metaObject()->propertyCount(); i++)
  {
    mChildren.push_back(
          std::make_unique<qt_property_node>(obj, obj.metaObject()->property(i), mDevice, *this));
  }

}

void qt_object_node::childEvent(QChildEvent* event)
{
  if(event->type() == QChildEvent::ChildAdded)
  {

  }
  else if(event->type() == QChildEvent::ChildRemoved)
  {

  }
}

std::unique_ptr<net::node_base> qt_object_node::makeChild(const std::string& name)
{
  return nullptr;
}

void qt_object_node::removingChild(net::node_base&)
{
}



qt_property_node::qt_property_node(
    QObject& obj,
    QMetaProperty p,
    net::device_base& device,
    net::node_base& parent):
  generic_node_base{p.name(), device, parent},
  generic_address{parent},
  m_obj{obj},
  m_prop{p}
{
  auto signature = QMetaObject::normalizedSignature("qtValueChanged(QVariant)");
  qDebug() << "Signature: " << signature;
  auto cb_idx = this->metaObject()->indexOfMethod(signature);
  auto cb = this->metaObject()->method(cb_idx);
  if(p.hasNotifySignal())
  {
    auto sig = p.notifySignal();
    bool ok = QMetaObject::checkConnectArgs(cb, sig);
    qDebug() << sig.methodSignature() << cb.methodSignature() << ok;
    if(ok)
      connect(&obj, p.notifySignal(), this, cb);
  }

  switch(p.type())
  {
    case QVariant::Bool:
      setValueType(ossia::val_type::BOOL);
      break;
    case QVariant::Time:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::ULongLong:
      setValueType(ossia::val_type::INT);
      break;
    case QVariant::Char:
      setValueType(ossia::val_type::CHAR);
      break;
    case QVariant::String:
    case QVariant::ByteArray:
      setValueType(ossia::val_type::STRING);
      break;
    case QVariant::Double:
      setValueType(ossia::val_type::FLOAT);
      break;
    case QVariant::Color:
      setValueType(ossia::val_type::INT);
      break;
    case QVariant::Point:
    case QVariant::PointF:
    case QVariant::Vector2D:
      setUnit(ossia::cartesian_2d_u{});
      break;
    case QVariant::Vector3D:
      setUnit(ossia::cartesian_3d_u{});
      break;
    case QVariant::Vector4D:
      setUnit(ossia::axis_u{});
      break;
    case QVariant::Quaternion:
      setUnit(ossia::quaternion_u{});
      break;
    case QVariant::Line:
    case QVariant::LineF:
    case QVariant::Rect:
    case QVariant::RectF:
    case QVariant::Size:
    case QVariant::SizeF:
      setValueType(ossia::val_type::VEC4F);
      break;
    case QVariant::List:
    case QVariant::StringList:
      setValueType(ossia::val_type::TUPLE);
      break;
    case QVariant::Date:
    default:
      setValueType(ossia::val_type::IMPULSE);
      break;
  }
}

net::generic_address&qt_property_node::setValue(const value& val)
{
  setValueQuiet(val);
  qDebug() << QString::fromStdString(ossia::value_to_pretty_string(val));
  m_prop.write(&m_obj, 0);
  send(cloneValue());
  return *this;
}

net::address_base* qt_property_node::getAddress() const
{
  return const_cast<qt_property_node*>(this);
}

net::address_base*qt_property_node::createAddress(val_type type)
{
  return nullptr;
}

bool qt_property_node::removeAddress()
{
  return false;
}

ossia::value to_val(QVariant v)
{
  return 1234;
}

std::unique_ptr<net::node_base> qt_property_node::makeChild(const std::string& name)
{
  return nullptr;
}

void qt_property_node::removingChild(net::node_base&)
{
}

qt_device::qt_device(
    QObject& obj,
    std::unique_ptr<net::protocol_base> protocol,
    std::string name):
  device_base(std::move(protocol)),
  m_root{obj, *this}
{
  mProtocol->setDevice(*this);
  mCapabilities.change_tree = false;
  m_root.setName(name);
}

const net::node_base&qt_device::getRootNode() const
{
  return m_root;
}

net::node_base&qt_device::getRootNode()
{
  return m_root;
}

}
}
