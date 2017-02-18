#include "qt_object_node.hpp"
#include "qt_property_node.hpp"
#include <QChildEvent>
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

  // Note : we create the childrens, and then lock the vector
  // because the children creation operation calls node_base::children() which causes
  // double locking.
  decltype(m_children) children_vect;
  for(auto c : obj.children())
  {
    children_vect.push_back(
          std::make_unique<qt_object_node>(*c, mDevice, *this));
  }

  for(int i = 0; i < obj.metaObject()->propertyCount(); i++)
  {
    children_vect.push_back(
          std::make_unique<qt_property_node>(obj, obj.metaObject()->property(i), mDevice, *this));
  }

  {
    write_lock_t lock{m_mutex};
    std::move(children_vect.begin(), children_vect.end(), std::back_inserter(m_children));
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
    write_lock_t write_lock{m_mutex};
    auto it = ossia::find_if(m_children, [=] (const auto& ptr) {
      auto p = ptr.get();
      if(auto po = dynamic_cast<qt_object_node*>(p))
      {
        if(&po->object() == event->child())
          return true;
      }
      return false;
    });

    if(it != m_children.end())
    {
      mDevice.onNodeRemoving(**it);
      m_children.erase(it);
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

}
}
