// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "qt_object_node.hpp"
#include "qt_property_node.hpp"
#include <QChildEvent>
#include <wobjectimpl.h>
W_OBJECT_IMPL(ossia::qt::qt_object_node)
namespace ossia
{
namespace qt
{

qt_object_node::qt_object_node(QObject& obj, net::device_base& device)
    : generic_node_base{{}, device}, mObject{obj}
{
  init(obj);
}

qt_object_node::qt_object_node(
    QObject& obj, net::device_base& device, net::node_base& aParent)
    : generic_node_base{{}, device, aParent}, mObject{obj}
{
  init(obj);
}

net::parameter_base* qt_object_node::get_parameter() const
{
  return mAddress.get();
}

net::parameter_base* qt_object_node::create_parameter(val_type type)
{
  return nullptr;
}

bool qt_object_node::remove_parameter()
{
  return false;
}

void qt_object_node::init(QObject& obj)
{
  auto name = obj.objectName();

  if (!name.isEmpty())
    set_name(obj.objectName().toStdString());
  else
  {
    std::string str;
    const QMetaObject* mo = obj.metaObject();
    while (str.empty())
    {
      str = mo->className();
      mo = mo->superClass();
      if (!mo)
        break;
    }

    if (!str.empty())
    {
      set_name(std::move(str));
    }
    else
    {
      set_name("Object");
    }
  }

  // Note : we create the childrens, and then lock the vector
  // because the children creation operation calls node_base::children() which
  // causes
  // double locking.
  decltype(m_children) children_vect;
  for (auto c : obj.children())
  {
    children_vect.push_back(
        std::make_unique<qt_object_node>(*c, m_device, *this));
  }

  for (int i = 0; i < obj.metaObject()->propertyCount(); i++)
  {
    children_vect.push_back(std::make_unique<qt_property_node>(
        obj, obj.metaObject()->property(i), m_device, *this));
  }

  {
    write_lock_t lock{m_mutex};
    std::move(
        children_vect.begin(), children_vect.end(),
        std::back_inserter(m_children));
  }
}

void qt_object_node::childEvent(QChildEvent* event)
{
  if (event->type() == QChildEvent::ChildAdded)
  {
    add_child(
        std::make_unique<qt_object_node>(*event->child(), m_device, *this));
  }
  else if (event->type() == QChildEvent::ChildRemoved)
  {
    write_lock_t write_lock{m_mutex};
    auto it = ossia::find_if(m_children, [=](const auto& ptr) {
      auto p = ptr.get();
      if (auto po = dynamic_cast<qt_object_node*>(p))
      {
        if (&po->object() == event->child())
          return true;
      }
      return false;
    });

    if (it != m_children.end())
    {
      m_device.on_node_removing(**it);
      m_children.erase(it);
    }
  }
}

std::unique_ptr<net::node_base>
qt_object_node::make_child(const std::string& name)
{
  return nullptr;
}

void qt_object_node::removing_child(net::node_base&)
{
}
}
}
