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

    if (x->m_otype == object_class::view)
      derived_classname = gensym("ossia.model");
    else if (x->m_otype == object_class::model)
      derived_classname = gensym("ossia.view");
    else if (x->m_otype == object_class::device)
      derived_classname = gensym("ossia.client");
    else if (x->m_otype == object_class::client)
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

ossia::max::address_scope get_parameter_type(const std::string& addr)
{
  address_scope type = address_scope::relative;
  if ( addr.length() > 0 )
  {
    if (addr[0] == '/')
      type = address_scope::absolute;
    else if ( addr.find(":/") != std::string::npos )
      type = address_scope::global;
  }
  return type;
}

std::vector<ossia::value> attribute2value(t_atom* atom, long size)
{
  std::vector<ossia::value> list;

  for (int i = 0; i < size; i++)
  {
    if (atom[i].a_type == A_FLOAT)
      list.push_back(atom_getfloat(&atom[i]));
    else if (atom[i].a_type == A_SYM)
      list.push_back(std::string(atom_getsym(&atom[i])->s_name));
  }
  return list;
}

ossia::val_type symbol2val_type(t_symbol* s)
{
  std::string type = s->s_name;

  if (type == "float")
    return ossia::val_type::FLOAT;
  else if (type == "symbol" || type == "string")
    return ossia::val_type::STRING;
  else if (type == "int")
    return ossia::val_type::INT;
  else if (type == "vec2f")
    return ossia::val_type::VEC2F;
  else if (type == "vec3f")
    return ossia::val_type::VEC3F;
  else if (type == "vec4f")
    return ossia::val_type::VEC4F;
  else if (type == "impulse")
    return ossia::val_type::IMPULSE;
  else if (type == "bool")
    return ossia::val_type::BOOL;
  else if (type == "list")
    return ossia::val_type::LIST;
  else if (type == "char")
    return ossia::val_type::CHAR;
  else
    return ossia::val_type::NONE;
}

t_symbol* val_type2symbol(ossia::val_type type)
{
  switch (type)
  {
    case ossia::val_type::FLOAT:
      return gensym("float");
      break;
    case ossia::val_type::INT:
      return gensym("int");
      break;
    case ossia::val_type::VEC2F:
      return gensym("vec2f");
      break;
    case ossia::val_type::VEC3F:
      return gensym("vec3f");
      break;
    case ossia::val_type::VEC4F:
      return gensym("vec4f");
      break;
    case ossia::val_type::IMPULSE:
      return gensym("impulse");
      break;
    case ossia::val_type::BOOL:
      return gensym("bool");
      break;
    case ossia::val_type::STRING:
      return gensym("string");
      break;
    case ossia::val_type::LIST:
      return gensym("list");
      break;
    case ossia::val_type::CHAR:
      return gensym("char");
      break;
    case ossia::val_type::NONE:
    default:
      return gensym("none");
  }
}

} // namespace max
} // namespace ossia
