// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "utils.hpp"

#include <ossia/network/common/path.hpp>

namespace ossia
{
namespace pd
{

std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol)
{
  std::vector<std::string> tags;

  if (tags_symbol)
  {
    char* c = tags_symbol->s_name;
    std::string tag = "";

    while (*c != '\0')
    {
      if (*c == ' ')
      {
        tags.push_back(tag);
        tag = std::string("");
      }
      else
        tag += *c;
      c++;
    }
    tags.push_back(tag);
  }
  return tags;
}

std::string string_from_path(const std::vector<std::string>& vs, fmt::MemoryWriter& fullpath)
{
  auto rit = vs.rbegin();
  for (; rit != vs.rend(); ++rit)
  {
    fullpath << "/" << *rit;
  }
  if (vs.empty())
    fullpath << "/";

  return fullpath.str();
}

void register_quarantinized()
{
  for (auto model : t_model::quarantine().copy())
  {
    obj_register<t_model>(model);
  }
  for (auto param : t_param::quarantine().copy())
  {
    obj_register<t_param>(param);
  }
  for (auto view : t_view::quarantine().copy())
  {
    obj_register<t_view>(view);
  }
  for (auto remote : t_remote::quarantine().copy())
  {
    obj_register<t_remote>(remote);
  }
}

t_obj_base* find_parent(t_eobj* x, std::string classname, int start_level, int* level)
{
  t_canvas* canvas = x->o_canvas;

  *level = start_level;

  while (canvas && start_level)
  {
    canvas = canvas->gl_owner; // gl_owner seems to be corrupted on the root
                               // canvas : canvas has no value
    start_level--;
  }

  if (start_level > 0)
    return nullptr; // if we can't reach start level (because we reach the root
                    // canvas before the start_level) then abort

  while (canvas != 0)
  {
    t_gobj* list = canvas->gl_list;
    while (list)
    {
      std::string current = list->g_pd->c_name->s_name;
      if ((current == classname) && (&(list->g_pd) != &(x->o_obj.te_g.g_pd)))
      { // check if type match and not the same intance...
        return (t_obj_base*) &(list->g_pd);
      }
      list = list->g_next;
    }
    canvas = canvas->gl_owner;
    (*level)++;
  }
  return nullptr;
}


ossia::net::node_base* find_parent_node(t_obj_base* x){
  int l;
  t_device* device = (t_device*)find_parent_alive(&x->m_obj, "ossia.device", 0, &l);
  t_client* client = (t_client*)find_parent_alive(&x->m_obj, "ossia.client", 0, &l);

  t_model* model = nullptr;
  t_view* view = nullptr;
  int view_level = 0, model_level = 0;
  int start_level = 0;

  if (x->m_otype == Type::view || x->m_otype == Type::model)
  {
    start_level = 1;
  }

  if (x->m_addr_scope == AddrScope::relative)
  {
    // then try to locate a parent view or model
    if (x->m_otype == Type::view || x->m_otype == Type::remote)
    {
      view
          = (t_view*)find_parent_alive(&x->m_obj, "ossia.view", start_level, &view_level);
    }

    if (!view)
    {
      model = (t_model*)find_parent_alive(
          &x->m_obj, "ossia.model", 0, &model_level);
    }
  }

  ossia::net::node_base* node = nullptr;

  if (view)
  {
    node = view->m_node;
  }
  else if (model)
  {
    node = model->m_node;
  }
  else if (client)
  {
    node = client->m_node;
  }
  else if (device)
  {
    node = device->m_node;
  }
  else
  {
    node = &ossia_pd::get_default_device()->get_root_node();
  }

  return node;
}

std::vector<t_obj_base*> find_child_to_register(
    t_obj_base* x, t_gobj* start_list, const std::string& classname, bool* found_dev)
{
  std::string subclassname
      = classname == "ossia.model" ? "ossia.param" : "ossia.remote";

  t_gobj* list = start_list;
  std::vector<t_obj_base*> found;
  bool found_model = false;
  bool found_view = false;

  // 1: iterate object list and look for ossia.model / ossia.view object
  while (list && list->g_pd)
  {
    std::string current = list->g_pd->c_name->s_name;
    if (current == classname)
    {
      t_obj_base* o;
      o = (t_obj_base*)&list->g_pd;
      if (x != o && !o->m_dead)
      {
        found.push_back(o);
      }
    }

    // if we're looking for ossia.view but found a model, remind it
    if ( classname == "ossia.view" && current == "ossia.model" )
      found_model = true;
    else if ( classname == "ossia.model" && current == "ossia.view" )
      found_view = true;

    // if there is a client or device in the current patcher
    // don't register anything
    if ( found_dev && (current == "ossia.device" || current == "ossia.client") )
    {
      t_obj_base* o;
      o = (t_obj_base*)&list->g_pd;
      if (x != o && !o->m_dead)
      {
        *found_dev = true;
      }
    }

    list = list->g_next;
  }

  // 2: if there is no ossia.model / ossia.view in the current patch, look into
  // the subpatches
  // if we found no ossia.view, but a ossia.model,
  // then remote in subpatchers should have been already register to model

  if (found.empty() && !found_model)
  {
    list = start_list;
    while (list && list->g_pd)
    {
      std::string current = list->g_pd->c_name->s_name;
      if (current == "canvas")
      {
        t_canvas* canvas = (t_canvas*)&list->g_pd;
        if (!canvas_istable(canvas))
        {
          t_gobj* _list = canvas->gl_list;
          bool _found_dev = false;
          std::vector<t_obj_base*> found_tmp
              = find_child_to_register(x, _list, classname, &_found_dev);
          if (!_found_dev)
          {
            found.insert(found.end(),found_tmp.begin(), found_tmp.end());
          }
        }
      }
      list = list->g_next;
    }

    // 3: finally look for ossia.param / ossia.remote in the same pather
    list = start_list;
    while (list && list->g_pd)
    {
      std::string current = list->g_pd->c_name->s_name;

      // if there is no view next to model, then take also remote into account
      if ( current == subclassname
          || ( !found_view && current == "ossia.remote" ) )
      {
        t_obj_base* o;
        o = (t_obj_base*)&list->g_pd;
        if (x != o && !o->m_dead)
        {
          found.push_back(o);
        }
      }
      list = list->g_next;
    }
  }

  return found;
}

bool find_peer(t_obj_base* x)
{
  t_symbol* classname = x->m_obj.o_obj.te_g.g_pd->c_name;
  t_symbol* derived_classname = nullptr;

  if (x->m_otype == Type::view)
    derived_classname = gensym("ossia.model");
  else if (x->m_otype == Type::model)
    derived_classname = gensym("ossia.view");
  else if (x->m_otype == Type::device)
    derived_classname = gensym("ossia.client");
  else if (x->m_otype == Type::client)
    derived_classname = gensym("ossia.device");

  t_gobj* list = x->m_obj.o_canvas->gl_list;
  while (list)
  {
    t_symbol* current = list->g_pd->c_name;
    if (current == classname)
    {
      if (x != (t_obj_base*)&list->g_pd)
      {
        return true;
      }
    }
    if (derived_classname && current == derived_classname)
      return true;
    list = list->g_next;
  }
  return false;
}

std::vector<ossia::net::node_base*> find_global_nodes(const std::string& addr)
{
  std::vector<ossia::net::node_base*> nodes{};
  auto& instance = ossia_pd::instance();
  size_t pos = addr.find(":");
  if (pos == std::string::npos) return nodes;

  std::string prefix = addr.substr(0,pos);
  // remove 'device_name:/' prefix
  std::string osc_name = addr.substr(pos+2);

  bool is_prefix_pattern = ossia::traversal::is_pattern(prefix);
  bool is_osc_name_pattern = ossia::traversal::is_pattern(osc_name);
  std::regex pattern(prefix.c_str());

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
        error("'%s' bad regex: %s", prefix.c_str(), e.what());
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
        error("'%s' bad regex: %s", prefix.c_str(), e.what());
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

ossia::pd::AddrScope get_address_scope(const std::string& addr)
{
  AddrScope type = AddrScope::relative;
  if ( addr.length() > 0 )
  {
    if (addr[0] == '/'
        && addr.length() > 1 && addr[1] != '/') // escape the '//' special combinaison
      type = AddrScope::absolute;
    else if ( addr.find(":/") != std::string::npos )
      type = AddrScope::global;
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
    else if (atom[i].a_type == A_SYMBOL)
      list.push_back(std::string(atom_getsymbol(&atom[i])->s_name));
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
    return ossia::val_type::TUPLE;
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
    case ossia::val_type::TUPLE:
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

}
}
