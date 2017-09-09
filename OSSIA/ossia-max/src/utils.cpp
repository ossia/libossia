#include "utils.hpp"

#include <ossia/network/common/path.hpp>
#include <boost/algorithm/string/predicate.hpp>


namespace ossia
{
namespace max
{

bool find_peer(object_base* x)
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
           && (object_base*)obj != x)
          return true;
        if (derived_classname && current == derived_classname)
          return true;
      }
    }
    return false;
}

std::vector<ossia::net::node_base*> find_global_nodes(ossia::string_view addr)
{
  std::vector<ossia::net::node_base*> nodes{};
  auto& instance = ossia_max::instance();
  size_t pos = addr.find(":");
  if (pos == std::string::npos) return nodes;

  ossia::string_view prefix = addr.substr(0,pos);
  // remove 'device_name:/' prefix
  ossia::string_view osc_name = addr.substr(pos+2);

  bool is_prefix_pattern = ossia::traversal::is_pattern(prefix);
  bool is_osc_name_pattern = ossia::traversal::is_pattern(osc_name);
  std::regex pattern(prefix.data(), prefix.size());

  for (auto device : instance.devices.copy())
  {
    auto dev = device->m_device;
    if (!dev) continue;

    std::string name = dev->get_name();

    bool match;
    if(is_prefix_pattern)
    {
      try {
        match = std::regex_match(name, pattern);
      } catch (std::exception& e) {
        error("'%s' bad regex: %s", prefix.data(), e.what());
        return nodes;
      }
    } else match = (name == prefix);

    if (match)
    {
      if (is_osc_name_pattern)
      {
        auto tmp = ossia::net::find_nodes(dev->get_root_node(), osc_name);
        nodes.insert(nodes.end(), tmp.begin(), tmp.end());
      }
      else
      {
        auto node = ossia::net::find_node(dev->get_root_node(),osc_name);
        if (node) nodes.push_back(node);
      }
    }
  }

  for (auto client : instance.clients.copy())
  {
    auto dev = client->m_device;
    if (!dev) continue;

    std::string name = dev->get_name();

    bool match;
    if(is_prefix_pattern)
    {
      try {
        match = std::regex_match(name, pattern);
      } catch (std::exception& e) {
        error("'%s' bad regex: %s", prefix.data(), e.what());
        return nodes;
      }
    } else match = (name == prefix);

    if (match)
    {
      if (is_osc_name_pattern)
      {
        auto tmp = ossia::net::find_nodes(dev->get_root_node(), osc_name);
        nodes.insert(nodes.end(), tmp.begin(), tmp.end());
      }
      else
      {
        auto node = ossia::net::find_node(dev->get_root_node(),osc_name);
        if (node) nodes.push_back(node);
      }
    }
  }
  return nodes;
}

ossia::max::address_scope get_address_scope(ossia::string_view addr)
{
  address_scope type = address_scope::relative;
  if (boost::starts_with(addr, "//") )
    type = address_scope::relative;
  else if ( boost::starts_with(addr, "/") )
    type = address_scope::absolute;
  else if ( addr.find(":/") != std::string::npos )
      type = address_scope::global;
  return type;
}

std::vector<ossia::value> attribute2value(t_atom* atom, long size)
{
  std::vector<ossia::value> list;
  list.reserve(size);

  for (int i = 0; i < size; i++)
  {
    if (atom[i].a_type == A_FLOAT)
      list.push_back(atom_getfloat(&atom[i]));
    else if (atom[i].a_type == A_LONG)
      list.push_back(static_cast<int>(atom_getlong(&atom[i])));
    else if (atom[i].a_type == A_SYM)
      list.push_back(std::string(atom_getsym(&atom[i])->s_name));
  }
  return list;
}

ossia::val_type symbol2val_type(t_symbol* s)
{
  if (s)
  {
    ossia::string_view type = s->s_name;

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
  } else
    return ossia::val_type::FLOAT;
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

ossia::bounding_mode symbol2bounding_mode(t_symbol* bounding_mode)
{
  if (bounding_mode == gensym("free"))
    return ossia::bounding_mode::FREE;
  else if (bounding_mode == gensym("both"))
    return ossia::bounding_mode::CLIP;
  else if (bounding_mode == gensym("wrap"))
    return ossia::bounding_mode::WRAP;
  else if (bounding_mode == gensym("fold"))
    return ossia::bounding_mode::FOLD;
  else if (bounding_mode == gensym("low"))
    return ossia::bounding_mode::LOW;
  else if (bounding_mode == gensym("high"))
    return ossia::bounding_mode::HIGH;
  else
  {
    error("unknown clip mode: %s", bounding_mode->s_name);
    return ossia::bounding_mode::FREE;
  }
}

t_symbol* bounding_mode2symbol(ossia::bounding_mode bm)
{
  switch (bm)
  {
    case ossia::bounding_mode::FREE:
      return gensym("free");
    case ossia::bounding_mode::CLIP:
      return gensym("both");
    case ossia::bounding_mode::WRAP:
      return gensym("wrap");
    case ossia::bounding_mode::FOLD:
      return gensym("fold");
    case ossia::bounding_mode::LOW:
      return gensym("low");
    case ossia::bounding_mode::HIGH:
      return gensym("high");
    default :
      return nullptr;
  }
}

ossia::access_mode symbol2access_mode(t_symbol* access_mode)
{
  if (access_mode == gensym("bi") || access_mode == gensym("rw"))
    return ossia::access_mode::BI;
  else if (access_mode == gensym("get") || access_mode == gensym("r"))
    return ossia::access_mode::GET;
  else if (access_mode == gensym("set") || access_mode == gensym("w"))
    return ossia::access_mode::SET;
  else
  {
    error("unknown access mode: %s", access_mode->s_name);
    return ossia::access_mode::BI;
  }
}

t_symbol* access_mode2symbol(ossia::access_mode mode)
{
  switch(mode)
  {
    case ossia::access_mode::SET:
      return gensym("set");
    case ossia::access_mode::GET:
      return gensym("get");
    default:
      return gensym("bi");
  }
}

} // namespace max
} // namespace ossia
