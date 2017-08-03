#include "utils.hpp"
#include "device.hpp"
#include "client.hpp"

namespace ossia
{
namespace max
{

bool find_peer(t_object_base* x)
{
    t_symbol* classname = object_classname(x);
    t_symbol* derived_classname = nullptr;

    if (x->m_otype == Type::view)
      derived_classname = gensym("ossia.model");
    else if (x->m_otype == Type::model)
      derived_classname = gensym("ossia.view");
    else if (x->m_otype == Type::device)
      derived_classname = gensym("ossia.client");
    else if (x->m_otype == Type::client)
      derived_classname = gensym("ossia.device");


    t_object *patcher, *box, *obj;
    object_obex_lookup(x, gensym("#P"), &patcher);
    for (box = jpatcher_get_firstobject(patcher); box; box =
         jbox_get_nextobject(box)) {
      obj = jbox_get_object(box);
      if (obj)
      {
        t_symbol* current = object_classname(obj);
        if(current == classname
           && (t_object_base*)obj != x)
          return true;
        if (derived_classname && current == derived_classname)
          return true;
      }
    }
    return false;
}

ossia::net::node_base* find_global_node(const std::string& addr)
{
  auto& instance = ossia_max::instance();
  for (auto device : instance.devices.copy())
  {
    auto dev = device->m_device;
    std::string name = dev->get_name();
    size_t pos = addr.find(":");
    std::string prefix = addr.substr(0,pos);
    if (pos != std::string::npos && name == prefix)
    {
      // remove 'device_name:/' prefix
      std::string osc_name = addr.substr(name.length()+2);
      auto node = ossia::net::find_node(dev->get_root_node(),osc_name);
      if (node) return node;
    }
  }

  for (auto client : instance.clients.copy())
  {
    auto dev = client->m_device;
    std::string name = dev->get_name();
    size_t pos = addr.find(":");
    std::string prefix = addr.substr(0,pos);
    if (pos != std::string::npos && name == prefix)
    {
      // remove 'device_name:/' prefix
      std::string osc_name = addr.substr(name.length()+2);
      auto node = ossia::net::find_node(dev->get_root_node(),osc_name);
      if (node) return node;
    }
  }
  return nullptr;
}

ossia::max::AddrType get_address_type(const std::string& addr)
{
  AddrType type = AddrType::relative;
  if ( addr.length() > 0 )
  {
    if (addr[0] == '/')
      type = AddrType::absolute;
    else if ( addr.find(":/") != std::string::npos )
      type = AddrType::global;
  }
  return type;
}

} // namespace max
} // namespace ossia
