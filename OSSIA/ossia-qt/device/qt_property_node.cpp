// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qt_property_node.hpp"
#include <ossia-qt/js_utilities.hpp>
namespace ossia
{
namespace qt
{

static tsl::hopscotch_map<int, QMetaMethod>& methods()
{
  static const auto method_offset
      = qt_property_node::staticMetaObject.indexOfMethod(
          "qtBoolValueChanged(bool)");
  static tsl::hopscotch_map<int, QMetaMethod> m{
      {QVariant::Bool,
       qt_property_node::staticMetaObject.method(method_offset + 0)},
      {QVariant::Time,
       qt_property_node::staticMetaObject.method(method_offset + 1)},
      {QVariant::Int,
       qt_property_node::staticMetaObject.method(method_offset + 2)},
      {QVariant::UInt,
       qt_property_node::staticMetaObject.method(method_offset + 3)},
      {QVariant::ULongLong,
       qt_property_node::staticMetaObject.method(method_offset + 4)},
      {QVariant::Char,
       qt_property_node::staticMetaObject.method(method_offset + 5)},
      {QVariant::String,
       qt_property_node::staticMetaObject.method(method_offset + 6)},
      {QVariant::ByteArray,
       qt_property_node::staticMetaObject.method(method_offset + 7)},
      {QVariant::Double,
       qt_property_node::staticMetaObject.method(method_offset + 8)},
      {QVariant::Color,
       qt_property_node::staticMetaObject.method(method_offset + 9)},
      {QVariant::Point,
       qt_property_node::staticMetaObject.method(method_offset + 10)},
      {QVariant::PointF,
       qt_property_node::staticMetaObject.method(method_offset + 11)},
      {QVariant::Vector2D,
       qt_property_node::staticMetaObject.method(method_offset + 12)},
      {QVariant::Vector3D,
       qt_property_node::staticMetaObject.method(method_offset + 13)},
      {QVariant::Vector4D,
       qt_property_node::staticMetaObject.method(method_offset + 14)},
      {QVariant::Quaternion,
       qt_property_node::staticMetaObject.method(method_offset + 15)},
      {QVariant::Line,
       qt_property_node::staticMetaObject.method(method_offset + 16)},
      {QVariant::LineF,
       qt_property_node::staticMetaObject.method(method_offset + 17)},
      {QVariant::Rect,
       qt_property_node::staticMetaObject.method(method_offset + 18)},
      {QVariant::RectF,
       qt_property_node::staticMetaObject.method(method_offset + 19)},
      {QVariant::Size,
       qt_property_node::staticMetaObject.method(method_offset + 20)},
      {QVariant::SizeF,
       qt_property_node::staticMetaObject.method(method_offset + 21)},
      {QVariant::List,
       qt_property_node::staticMetaObject.method(method_offset + 22)},
      {QVariant::StringList,
       qt_property_node::staticMetaObject.method(method_offset + 23)},
      {QVariant::Date,
       qt_property_node::staticMetaObject.method(method_offset + 24)},
      {QVariant::Invalid,
       qt_property_node::staticMetaObject.method(
           method_offset + 25)} // impulse
  };

  return m;
}

qt_property_node::qt_property_node(
    QObject& obj, QMetaProperty p, net::device_base& device,
    net::node_base& parent)
    : generic_node_base{p.name(), device, parent}
    , generic_address{parent}
    , m_obj{obj}
    , m_prop{p}
{
  if (p.hasNotifySignal())
  {
    connectSignalToMatchingMethod(p.notifySignal(), methods(), &obj, this);
  }

  set_address_type(p.type(), *this);
  ossia::net::generic_address::set_value_quiet(qt_to_ossia{}(p.read(&obj)));
  ;

  connect(
      this, &qt_property_node::setValue_sig, this,
      &qt_property_node::setValue_slot, Qt::QueuedConnection);
}

void qt_property_node::setValue_slot(const ossia::value& ossia_val)
{
  auto cur = m_prop.read(&m_obj);
  auto next = ossia_to_qvariant{}(m_prop.type(), ossia_val);
  if (cur != next)
    m_prop.write(&m_obj, next);
}

void qt_property_node::set_value_quiet(const ossia::value& ossia_val)
{
  ossia::net::generic_address::set_value_quiet(ossia_val);

  emit setValue_sig(ossia_val);
}
void qt_property_node::set_value_quiet(ossia::value&& ossia_val)
{
  ossia::net::generic_address::set_value_quiet(std::move(ossia_val));

  emit setValue_sig(ossia_val);
}

net::address_base* qt_property_node::get_address() const
{
  return const_cast<qt_property_node*>(this);
}

net::address_base* qt_property_node::create_address(val_type type)
{
  return nullptr;
}

bool qt_property_node::remove_address()
{
  return false;
}

std::unique_ptr<net::node_base>
qt_property_node::make_child(const std::string& name)
{
  return nullptr;
}

void qt_property_node::removing_child(net::node_base&)
{
}
}
}
