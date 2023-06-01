// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qt_property_node.hpp"

#include <ossia-qt/js_utilities.hpp>

#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::qt::qt_property_node)
namespace ossia
{
namespace qt
{

static ossia::hash_map<int, QMetaMethod>& methods()
{
  static const auto method_offset
      = qt_property_node::staticMetaObject.indexOfMethod("qtBoolValueChanged(bool)");
  static ossia::hash_map<int, QMetaMethod> m{
      {QMetaType::Type::Bool,
       qt_property_node::staticMetaObject.method(method_offset + 0)},
      {QMetaType::Type::QTime,
       qt_property_node::staticMetaObject.method(method_offset + 1)},
      {QMetaType::Type::Int,
       qt_property_node::staticMetaObject.method(method_offset + 2)},
      {QMetaType::Type::UInt,
       qt_property_node::staticMetaObject.method(method_offset + 3)},
      {QMetaType::Type::ULongLong,
       qt_property_node::staticMetaObject.method(method_offset + 4)},
      {QMetaType::Type::Char,
       qt_property_node::staticMetaObject.method(method_offset + 5)},
      {QMetaType::Type::QString,
       qt_property_node::staticMetaObject.method(method_offset + 6)},
      {QMetaType::Type::QByteArray,
       qt_property_node::staticMetaObject.method(method_offset + 7)},
      {QMetaType::Type::Double,
       qt_property_node::staticMetaObject.method(method_offset + 8)},
      {QMetaType::Type::QColor,
       qt_property_node::staticMetaObject.method(method_offset + 9)},
      {QMetaType::Type::QPoint,
       qt_property_node::staticMetaObject.method(method_offset + 10)},
      {QMetaType::Type::QPointF,
       qt_property_node::staticMetaObject.method(method_offset + 11)},
      {QMetaType::Type::QVector2D,
       qt_property_node::staticMetaObject.method(method_offset + 12)},
      {QMetaType::Type::QVector3D,
       qt_property_node::staticMetaObject.method(method_offset + 13)},
      {QMetaType::Type::QVector4D,
       qt_property_node::staticMetaObject.method(method_offset + 14)},
      {QMetaType::Type::QQuaternion,
       qt_property_node::staticMetaObject.method(method_offset + 15)},
      {QMetaType::Type::QLine,
       qt_property_node::staticMetaObject.method(method_offset + 16)},
      {QMetaType::Type::QLineF,
       qt_property_node::staticMetaObject.method(method_offset + 17)},
      {QMetaType::Type::QRect,
       qt_property_node::staticMetaObject.method(method_offset + 18)},
      {QMetaType::Type::QRectF,
       qt_property_node::staticMetaObject.method(method_offset + 19)},
      {QMetaType::Type::QSize,
       qt_property_node::staticMetaObject.method(method_offset + 20)},
      {QMetaType::Type::QSizeF,
       qt_property_node::staticMetaObject.method(method_offset + 21)},
      {QMetaType::Type::QVariantList,
       qt_property_node::staticMetaObject.method(method_offset + 22)},
      {QMetaType::Type::QStringList,
       qt_property_node::staticMetaObject.method(method_offset + 23)},
      {QMetaType::Type::QDate,
       qt_property_node::staticMetaObject.method(method_offset + 24)},
      {QMetaType::Type::UnknownType,
       qt_property_node::staticMetaObject.method(method_offset + 25)} // impulse
  };

  return m;
}

qt_property_node::qt_property_node(
    QObject& obj, QMetaProperty p, net::device_base& device, net::node_base& parent)
    : generic_node_base{p.name(), device, parent}
    , generic_parameter{parent}
    , m_obj{obj}
    , m_prop{p}
{
  if(p.hasNotifySignal())
  {
    connectSignalToMatchingMethod(p.notifySignal(), methods(), &obj, this);
  }

  set_parameter_type((QMetaType::Type)p.typeId(), *this);
  ossia::net::generic_parameter::set_value_quiet(qt_to_ossia{}(p.read(&obj)));
  ;

  connect(
      this, &qt_property_node::setValue_sig, this, &qt_property_node::setValue_slot,
      Qt::QueuedConnection);
}

void qt_property_node::setValue_slot(const ossia::value& ossia_val)
{
  auto cur = m_prop.read(&m_obj);
  auto next = ossia_to_qvariant{}((QMetaType::Type)m_prop.typeId(), ossia_val);
  if(cur != next)
    m_prop.write(&m_obj, next);
}

ossia::value qt_property_node::set_value_quiet(const ossia::value& ossia_val)
{
  auto v = ossia::net::generic_parameter::set_value_quiet(ossia_val);

  setValue_sig(ossia_val);

  return v;
}
ossia::value qt_property_node::set_value_quiet(ossia::value&& ossia_val)
{
  auto v = ossia::net::generic_parameter::set_value_quiet(std::move(ossia_val));

  setValue_sig(ossia_val);

  return v;
}

net::parameter_base* qt_property_node::get_parameter() const
{
  return const_cast<qt_property_node*>(this);
}

net::parameter_base* qt_property_node::create_parameter(val_type type)
{
  return nullptr;
}

bool qt_property_node::remove_parameter()
{
  return false;
}

std::unique_ptr<net::node_base> qt_property_node::make_child(const std::string& name)
{
  return nullptr;
}

void qt_property_node::removing_child(net::node_base&) { }
}
}
