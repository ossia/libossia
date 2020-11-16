#include "utils.hpp"

#include <ossia/network/common/path.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "ossia-max.hpp"
#include <regex>

namespace ossia
{
namespace max
{

std::vector<ossia::net::generic_device*> get_all_devices()
{
  ossia_max& instance = ossia_max::instance();
  std::vector<ossia::net::generic_device*> devs;
  devs.reserve(instance.devices.size() + instance.clients.size() + 1);
  devs.push_back(instance.get_default_device().get());

  for (auto device_obj : instance.devices.reference())
  {
    auto dev = device_obj->m_device;
    if (dev)
      devs.push_back(dev.get());
  }

  for(auto client_obj : instance.clients.reference())
  {
    auto dev = client_obj->m_device;
    if (dev)
      devs.push_back(dev.get());
  }

  return devs;
}

std::vector<ossia::net::node_base*> find_or_create_global_nodes(ossia::string_view addr, bool create)
{
  size_t pos = addr.find(":");
  if (pos == std::string::npos) return {};

  ossia::string_view prefix = addr.substr(0,pos);
  // remove 'device_name:/' prefix
  ossia::string_view osc_name = addr.substr(pos+2);

  bool is_prefix_pattern = ossia::traversal::is_pattern(prefix);
  bool is_osc_name_pattern = ossia::traversal::is_pattern(osc_name);
  std::regex pattern(prefix.data(), prefix.size(), std::regex_constants::ECMAScript);

  std::vector<ossia::net::generic_device*> devs = get_all_devices();

  std::vector<ossia::net::node_base*> nodes{};
  nodes.reserve(4);

  for(auto dev : devs)
  {
    std::string name = dev->get_name();

    bool match;
    if(is_prefix_pattern)
    {
      try {
        match = std::regex_match(name, pattern);
      } catch (std::exception& e) {
        error("'%s' bad regex: %s", prefix.data(), e.what());
        return {};
      }
    } else match = (name == prefix);

    if (match)
    {
      if(create)
      {
        auto new_nodes = ossia::net::create_nodes(dev->get_root_node(), osc_name);
        nodes.insert(nodes.end(), new_nodes.begin(), new_nodes.end());
      }
      else
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
  }

  return nodes;
}

ossia::net::address_scope get_address_scope(ossia::string_view addr)
{
  ossia::net::address_scope type = ossia::net::address_scope::relative;
  if (boost::starts_with(addr, "//") )
    type = ossia::net::address_scope::relative;
  else if ( boost::starts_with(addr, "/") )
    type = ossia::net::address_scope::absolute;
  else if ( addr.find(":/") != std::string::npos )
      type = ossia::net::address_scope::global;
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

std::vector<ossia::max::matcher*> make_matchers_vector(object_base* x, const ossia::net::node_base* node)
{
  std::vector<ossia::max::matcher*> matchers;
  if (node)
  {
    for (auto& m : x->m_matchers)
    {
      if (node == m->get_node() && !m->is_zombie())
      {
        matchers.push_back(m.get());
        break;
      }
    }
  }

  /*
  if (matchers.empty())
  {
    matchers.reserve(x->m_matchers.size());
    for (auto& m : x->m_matchers)
      matchers.push_back(&m);
  }
  */

  return matchers;
}

ossia::value atom2value(t_symbol* s, int argc, t_atom* argv)
{
    if (argc == 1 && !s)
    {
      ossia::value v;
      // convert one element array to single element
      switch(argv->a_type)
      {
        case A_SYM:
          v = ossia::value(std::string(atom_getsym(argv)->s_name));
          break;
        case A_FLOAT:
          v = ossia::value(atom_getfloat(argv));
          break;
        case A_LONG:
          v = static_cast<int32_t>(atom_getlong(argv));
          break;
        default:
          ;
      }

      return v;
    }
    else
    {
      std::vector<ossia::value> list;
      list.reserve(argc+1);
      if ( s && s != gensym("list") )
        list.push_back(std::string(s->s_name));

      for (; argc > 0; argc--, argv++)
      {
        switch (argv->a_type)
        {
          case A_SYM:
            list.push_back(std::string(atom_getsym(argv)->s_name));
            break;
          case A_FLOAT:
            list.push_back(atom_getfloat(argv));
            break;
          case A_LONG:
            list.push_back(static_cast<int32_t>(atom_getlong(argv)));
            break;
          default:
            ;
        }
      }

      return ossia::value(list);
    }
}


template<class T> void register_objects_by_type(const ossia::safe_set<T>& objs)
{
  for(auto obj : objs)
  {
    obj->update_path();
    obj->do_registration();
  }
}

void register_children_in_patcher_recursively(t_object* patcher, object_base* caller)
{

  static int count = 0;
  count++;
  std::vector<object_base*> objects_to_register;

  ossia_max::instance().patchers[patcher].loadbanged = true;

  t_object* root_patcher{};
  if(caller)
    root_patcher = caller->m_patcher;

  // 1: look for device, client, model and view objects into the patcher
  auto& pat_desc = ossia_max::instance().patchers[patcher];

  if(root_patcher != patcher)
  {
    device_base* db = pat_desc.device?static_cast<device_base*>(pat_desc.device):
                                      static_cast<device_base*>(pat_desc.client);
    if(db && db != caller)
    {
      if(db->m_device)
      {
        db->m_registered = true;
        return register_children_in_patcher_recursively(patcher, db);
      }
      else
        return;
    }
  }

  node_base* nb = pat_desc.model?static_cast<node_base*>(pat_desc.model):
                                 static_cast<node_base*>(pat_desc.view);

  if(nb && nb != caller)
  {
    nb->update_path();
    switch(nb->m_otype)
    {
      case object_class::model:
      {
        auto mdl = static_cast<model*>(nb);
        mdl->do_registration();
        register_children_in_patcher_recursively(patcher, mdl);
        break;
      }
      case object_class::view:
      {
        auto vw = static_cast<view*>(nb);
        vw->do_registration();
        register_children_in_patcher_recursively(patcher, vw);
        break;
      }
      default:
        break;
    }
    return;
  }

  register_objects_by_type(pat_desc.parameters);
  register_objects_by_type(pat_desc.remotes);
  register_objects_by_type(pat_desc.attributes);

  for(auto subpatcher : pat_desc.subpatchers)
  {
    register_children_in_patcher_recursively(subpatcher, caller);
  }
  count--;

  // when the first call end, fire all values (when there is some)
  if(count = 0)
  {

  }
}

t_object* get_patcher(t_object* object)
{
  t_object* patcher = nullptr;
  auto err = object_obex_lookup(object, gensym("#P"), &patcher);

  auto bpatcher = object_attr_getobj(object, _sym_parentpatcher);

  if(patcher != nullptr && err == MAX_ERR_NONE)
    return patcher;
  else
    return bpatcher;
}

// FIXME move that to utils.cpp
std::vector<std::string> parse_tags_symbol(t_symbol** tags_symbol, long size)
{
  std::vector<std::string> tags;

  for(int i=0;i<size;i++)
  {
    tags.push_back(tags_symbol[i]->s_name);
  }

  return tags;
}

} // namespace max
} // namespace ossia
