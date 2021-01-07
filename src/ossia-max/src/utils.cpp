#include "utils.hpp"

#include <ossia/network/common/path.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "ossia-max.hpp"
#include <regex>
#include <algorithm> // std::lexicographical_compare

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


std::vector<parameter_base*> list_all_objects_recursively(t_object* patcher)
{
  std::vector<parameter_base*> objects;
  objects.reserve(128);
  auto& pat_desc = ossia_max::instance().patchers[patcher];

  objects.insert(objects.end(), pat_desc.parameters.begin(), pat_desc.parameters.end());
  objects.insert(objects.end(), pat_desc.remotes.begin(), pat_desc.remotes.end());

  for(auto subpat : pat_desc.subpatchers)
  {
    auto subpatcher_objects = list_all_objects_recursively(subpat);
    objects.insert(objects.end(), subpatcher_objects.begin(), subpatcher_objects.end());
  }
  return objects;
}

// return a vector of all priority from top to bottom (root node)
std::vector<ossia::net::priority> get_priority_list(ossia::net::node_base* node)
{
  // TODO cache that and reset on tree changed
  std::vector<float> priorities;
  priorities.reserve(32);

  while(node)
  {
    auto prio = ossia::net::get_priority(*node);
    if(prio)
    {
      priorities.push_back(*prio);
    }
    else
    {
      priorities.push_back(0);
    }
    node = node->get_parent();
  }

  std::reverse(priorities.begin(), priorities.end());

  return priorities;
}

void fire_values_by_priority(std::vector<node_priority>& priority_graph, bool only_default = false)
{
  // keep only BI and not impulse (with default value, optionally)
  ossia::remove_erase_if(priority_graph, [only_default](const node_priority& np){
    auto param = np.obj->get_node()->get_parameter();
    if(param
    && param->get_access() == ossia::access_mode::BI
    && param->get_value_type() != ossia::val_type::IMPULSE)
    {
      if(only_default)
      {
        auto val = param->get_default_value();
        if(!val)
          return true;
      }
      return false;
    }
    return true;
  });

  // sort vector against all priorities
  std::sort(priority_graph.begin(), priority_graph.end(), [](const node_priority& a, const node_priority& b){
    return std::lexicographical_compare(
        b.priorities.begin(), b.priorities.end(),
        a.priorities.begin(), a.priorities.end());
  });

  // fire values by descending priority order
  for(const auto& p : priority_graph)
  {
    auto matcher = p.obj;
    auto node = matcher->get_node();
    auto param = node->get_parameter();
    if(param)
    {
      if(only_default && p.obj->get_owner()->m_otype == object_class::param)
      {
        auto val = param->get_default_value();
        if(val)
          matcher->output_value(*val);
      }
      else
      {
        matcher->output_value(param->value());
      }
    }
  }
}

void output_all_values(t_object* patcher, bool only_default)
{
  auto all_objects = list_all_objects_recursively(patcher);

  std::vector<node_priority> priority_graph;
  priority_graph.reserve(all_objects.size());

  for(const auto obj : all_objects)
  {
    assert(obj != nullptr);

    for(const auto& m : obj->m_matchers)
    {
      auto node = m->get_node();
      if(node)
      {
        auto prio = get_priority_list(node);
        priority_graph.push_back({m, prio});
      }
    }
  }

  fire_values_by_priority(priority_graph, only_default);
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

// TODO wrap this in a member method and rename it get_matchers(node_base* n);
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
  // TODO unify with parameter_base::push code
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
    if(!obj->m_dead && obj->m_name && obj->m_name != _sym_nothing)
    {
      obj->m_node_selection.clear();
      obj->m_matchers.clear();
      obj->update_path();
      obj->do_registration();
    }
  }
}

void register_children_in_patcher_recursively(t_object* patcher, object_base* caller)
{
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
      if(db->m_device && !db->m_dead)
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
    if(!nb->m_dead && nb->m_name && nb->m_name != _sym_nothing)
    {
      nb->m_node_selection.clear();
      nb->m_matchers.clear();
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
