#include "qt_device.hpp"
#include <QEvent>
#include <QDebug>

namespace ossia
{
namespace qt
{

qt_object_node::qt_object_node(QObject& obj, net::device_base& device):
  generic_node_base{{}, device}
, mObject{obj}
{
  init(obj);
}

qt_object_node::qt_object_node(QObject& obj, net::device_base& device, net::node_base& aParent):
  generic_node_base{{}, device, aParent}
, mObject{obj}
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
  auto name = obj.objectName();

  if(!name.isEmpty())
    setName(obj.objectName().toStdString());
  else
  {
    std::string str;
    const QMetaObject* mo = obj.metaObject();
    while(str.empty())
    {
      str = mo->className();
      mo = mo->superClass();
      if(!mo)
        break;
    }

    if(!str.empty())
    {
      setName(std::move(str));
    }
    else
    {
      setName("Object");
    }
  }

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
    addChild(std::make_unique<qt_object_node>(*event->child(), mDevice, *this));
  }
  else if(event->type() == QChildEvent::ChildRemoved)
  {
    auto it = ossia::find_if(mChildren, [=] (const auto& ptr) {
      auto p = ptr.get();
      if(auto po = dynamic_cast<qt_object_node*>(p))
      {
        if(&po->object() == event->child())
          return true;
      }
      return false;
    });

    if(it != mChildren.end())
    {
      mDevice.onNodeRemoving(**it);
      mChildren.erase(it);
    }
  }
}

std::unique_ptr<net::node_base> qt_object_node::makeChild(const std::string& name)
{
  return nullptr;
}

void qt_object_node::removingChild(net::node_base&)
{
}


static tsl::hopscotch_map<QVariant::Type, QMetaMethod> methods = {
  {QVariant::Bool, qt_property_node::staticMetaObject.method(5 + 0)},
  {QVariant::Time, qt_property_node::staticMetaObject.method(5 + 1)},
  {QVariant::Int, qt_property_node::staticMetaObject.method(5 + 2)},
  {QVariant::UInt, qt_property_node::staticMetaObject.method(5 + 3)},
  {QVariant::ULongLong, qt_property_node::staticMetaObject.method(5 + 4)},
  {QVariant::Char, qt_property_node::staticMetaObject.method(5 + 5)},
  {QVariant::String, qt_property_node::staticMetaObject.method(5 + 6)},
  {QVariant::ByteArray, qt_property_node::staticMetaObject.method(5 + 7)},
  {QVariant::Double, qt_property_node::staticMetaObject.method(5 + 8)},
  {QVariant::Color, qt_property_node::staticMetaObject.method(5 + 9)},
  {QVariant::Point, qt_property_node::staticMetaObject.method(5 + 10)},
  {QVariant::PointF, qt_property_node::staticMetaObject.method(5 + 11)},
  {QVariant::Vector2D, qt_property_node::staticMetaObject.method(5 + 12)},
  {QVariant::Vector3D, qt_property_node::staticMetaObject.method(5 + 13)},
  {QVariant::Vector4D, qt_property_node::staticMetaObject.method(5 + 14)},
  {QVariant::Quaternion, qt_property_node::staticMetaObject.method(5 + 15)},
  {QVariant::Line, qt_property_node::staticMetaObject.method(5 + 16)},
  {QVariant::LineF, qt_property_node::staticMetaObject.method(5 + 17)},
  {QVariant::Rect, qt_property_node::staticMetaObject.method(5 + 18)},
  {QVariant::RectF, qt_property_node::staticMetaObject.method(5 + 19)},
  {QVariant::Size, qt_property_node::staticMetaObject.method(5 + 20)},
  {QVariant::SizeF, qt_property_node::staticMetaObject.method(5 + 21)},
  {QVariant::List, qt_property_node::staticMetaObject.method(5 + 22)},
  {QVariant::StringList, qt_property_node::staticMetaObject.method(5 + 23)},
  {QVariant::Date, qt_property_node::staticMetaObject.method(5 + 24)}
};
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
  if(p.hasNotifySignal())
  {
    auto sig = p.notifySignal();
    if(sig.parameterCount() == 1)
    {
      auto t = sig.parameterType(0);

      auto method_it = methods.find((QVariant::Type)t);
      if(method_it != methods.end())
      {
        connect(&obj, p.notifySignal(), this, method_it->second);
      }
    }
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
      setUnit(ossia::argb_u{});
      break;
    case QVariant::Point:
    case QVariant::PointF:
    case QVariant::Vector2D:
    case QVariant::Size:
    case QVariant::SizeF:
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

void qt_property_node::setValueQuiet(const value& ossia_val)
{
  ossia::net::generic_address::setValueQuiet(ossia_val);

  switch(m_prop.type())
  {
    case QVariant::Bool:
      m_prop.write(&m_obj, QVariant::fromValue(ossia_val.get<bool>()));
      break;
    case QVariant::Time:
      m_prop.write(&m_obj, QVariant::fromValue(QTime().addMSecs(ossia_val.get<int>())));
      break;
    case QVariant::Int:
      m_prop.write(&m_obj, QVariant::fromValue(ossia_val.get<int>()));
      break;
    case QVariant::UInt:
      m_prop.write(&m_obj, QVariant::fromValue((quint32)ossia_val.get<int>()));
      break;
    case QVariant::ULongLong:
      m_prop.write(&m_obj, QVariant::fromValue((qlonglong)ossia_val.get<int>()));
      break;
    case QVariant::Char:
      m_prop.write(&m_obj, QVariant::fromValue(QChar::fromLatin1(ossia_val.get<char>())));
      break;
    case QVariant::String:
      m_prop.write(&m_obj, QVariant::fromValue(QString::fromStdString(ossia_val.get<std::string>())));
      break;
    case QVariant::ByteArray:
      m_prop.write(&m_obj, QVariant::fromValue(QByteArray::fromStdString(ossia_val.get<std::string>())));
      break;
    case QVariant::Double:
      m_prop.write(&m_obj, QVariant::fromValue((double)ossia_val.get<float>()));
      break;
    case QVariant::Color:
    {
      auto val = ossia_val.get<Vec4f>();
      m_prop.write(&m_obj, QVariant::fromValue(QColor::fromRgbF(val[1], val[2], val[3], val[0])));
      break;
    }
    case QVariant::Point:
    {
      auto val = ossia_val.get<Vec2f>();
      m_prop.write(&m_obj, QVariant::fromValue(QPoint(val[0], val[1])));
      break;
    }
    case QVariant::PointF:
    {
      auto val = ossia_val.get<Vec2f>();
      m_prop.write(&m_obj, QVariant::fromValue(QPointF(val[0], val[1])));
      break;
    }
    case QVariant::Vector2D:
    {
      auto val = ossia_val.get<Vec2f>();
      m_prop.write(&m_obj, QVariant::fromValue(QVector2D(val[0], val[1])));
      break;
    }
      break;
    case QVariant::Vector3D:
    {
      auto val = ossia_val.get<Vec3f>();
      m_prop.write(&m_obj, QVariant::fromValue(QVector3D(val[0], val[1], val[2])));
      break;
    }
    case QVariant::Vector4D:
    {
      auto val = ossia_val.get<Vec4f>();
      m_prop.write(&m_obj, QVariant::fromValue(QVector4D(val[0], val[1], val[2], val[3])));
      break;
    }
    case QVariant::Quaternion:
    {
      auto val = ossia_val.get<Vec4f>();
      m_prop.write(&m_obj, QVariant::fromValue(QQuaternion(val[0], val[1], val[2], val[3])));
      break;
    }
    case QVariant::Line:
    {
      auto val = ossia_val.get<Vec4f>();
      m_prop.write(&m_obj, QVariant::fromValue(QLine(val[0], val[1], val[2], val[3])));
      break;
    }
    case QVariant::LineF:
    {
      auto val = ossia_val.get<Vec4f>();
      m_prop.write(&m_obj, QVariant::fromValue(QLineF(val[0], val[1], val[2], val[3])));
      break;
    }
    case QVariant::Rect:
    {
      auto val = ossia_val.get<Vec4f>();
      m_prop.write(&m_obj, QVariant::fromValue(QRect(val[0], val[1], val[2], val[3])));
      break;
    }
    case QVariant::RectF:
    {
      auto val = ossia_val.get<Vec4f>();
      m_prop.write(&m_obj, QVariant::fromValue(QRectF(val[0], val[1], val[2], val[3])));
      break;
    }
    case QVariant::Size:
    {
      auto val = ossia_val.get<Vec2f>();
      m_prop.write(&m_obj, QVariant::fromValue(QSize(val[0], val[1])));
      break;
    }
    case QVariant::SizeF:
    {
      auto val = ossia_val.get<Vec2f>();
      m_prop.write(&m_obj, QVariant::fromValue(QSizeF(val[0], val[1])));
      break;
    }
    case QVariant::List:
    {
      break;
    }
    case QVariant::StringList:
    {
      break;
    }
    case QVariant::Date:
    default:
      break;
  }
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
