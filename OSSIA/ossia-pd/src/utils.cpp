// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "utils.hpp"

#include <ossia/network/common/path.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace ossia
{
namespace pd
{

ossia::value atom2value(t_symbol* s, int argc, t_atom* argv)
{
    if (argc == 1 && !s)
    {
      ossia::value v;
      // convert one element array to single element
      switch(argv->a_type)
      {
        case A_SYMBOL:
          v = ossia::value(std::string(atom_getsymbol(argv)->s_name));
          break;
        case A_FLOAT:
          v = ossia::value(atom_getfloat(argv));
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
          case A_SYMBOL:
            list.push_back(std::string(atom_getsymbol(argv)->s_name));
            break;
          case A_FLOAT:
            list.push_back(atom_getfloat(argv));
            break;
          default:
            ;
        }
      }

      return ossia::value(list);
    }
}

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
  for (auto view : ossia::pd::view::quarantine().copy())
  {
    obj_register<ossia::pd::view>(view);
  }
  for (auto remote : ossia::pd::remote::quarantine().copy())
  {
    obj_register<ossia::pd::remote>(remote);
  }
}

std::string get_absolute_path(object_base* x)
{
  fmt::MemoryWriter fullpath;
  std::vector<std::string> vs;
  vs.reserve(8);

  vs.push_back(x->m_name->s_name);

  ossia::pd::view* view = nullptr;
  ossia::pd::model* model = nullptr;

  int view_level = 0;
  int start_level = 0;

  if (x->m_otype == object_class::view)
    start_level = 1;

  view = find_parent_alive<ossia::pd::view>(
      x, start_level, &view_level);

  if (x->m_otype == object_class::model
      || x->m_otype == object_class::remote)
  {
    model = find_parent_alive<ossia::pd::model>(
        x, start_level, &view_level);
  }

  t_eobj* obj = nullptr;

  // FIXME this will fail as soon as https://github.com/OSSIA/libossia/issues/208 is implemented
  // or if model and view are mixed in the same hierarchy

  ossia::pd::view* tmp_view = nullptr;
  while (view)
  {
    vs.push_back(view->m_name->s_name);
    tmp_view = view;
    view = find_parent_alive<ossia::pd::view>(tmp_view, 1, &view_level);
  }

  ossia::pd::model* tmp_model = nullptr;
  while (model)
  {
    vs.push_back(model->m_name->s_name);
    tmp_model = model;
    model = find_parent_alive<ossia::pd::model>(tmp_model, 1, &view_level);
  }
/*
  if(tmp_model)
    obj = &tmp_model->m_obj;
  else if (tmp_view)
    obj = &tmp_view->m_obj;
  else
    obj = &x->m_obj;

  int device_level = 0;
  int client_level = 0;

  // FIXme TODO use get root device instead
  auto device = (ossia::pd::device*)find_parent(obj, "ossia.device", 0, &device_level);
  auto client = (ossia::pd::client*)find_parent(obj, "ossia.client", 0, &client_level);

  if (client)
    fullpath << client->m_name->s_name << ":";
  if (device)
    fullpath << device->m_name->s_name << ":";
  else
    fullpath << ossia_pd::instance().get_default_device()->get_name() << ":";
  */

  return string_from_path(vs, fullpath);
}

std::vector<object_base*> find_child_to_register(object_base* x, t_gobj* start_list, t_symbol* classname)
{
  const t_symbol* subclassname
      = classname == ossia_pd::o_sym_model ? ossia_pd::o_sym_param : ossia_pd::o_sym_remote;

  t_gobj* list = start_list;
  std::vector<object_base*> found;
  found.reserve(4);

  bool found_model = false;
  bool found_view = false;

  // 1: iterate object list and look for ossia.model / ossia.view object
  while (list && list->g_pd)
  {
    const t_symbol* current = list->g_pd->c_name;
    if (current == classname)
    {
      object_base* o;
      o = (object_base*)&list->g_pd;
      if (x != o && !o->m_dead)
      {
        found.push_back(o);
      }
    }

    // if we're looking for ossia.view but found a model, remind it
    if ( classname == ossia_pd::o_sym_view && current == ossia_pd::o_sym_model )
      found_model = true;
    else if ( classname == ossia_pd::o_sym_model && current == ossia_pd::o_sym_view )
      found_view = true;

    // if there is a client or device in the current patcher
    // don't register anything
    if (current == ossia_pd::o_sym_device || current == ossia_pd::o_sym_client)
    {
      // but don't return if an object found itself
      object_base* o;
      o = (object_base*)&list->g_pd;
      if ( x != o )
        return {};
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
      const t_symbol* current = list->g_pd->c_name;
      if (current == gensym("canvas"))
      {
        t_canvas* canvas = (t_canvas*)&list->g_pd;
        if (!canvas_istable(canvas))
        {
          t_gobj* _list = canvas->gl_list;
          std::vector<object_base*> found_tmp
              = find_child_to_register(x, _list, classname);

          found.insert(found.end(),found_tmp.begin(), found_tmp.end());
        }
      }
      list = list->g_next;
    }

    // 3: finally look for ossia.param / ossia.remote in the same pather
    list = start_list;
    while (list && list->g_pd)
    {
      const t_symbol* current = list->g_pd->c_name;

      // if there is no view next to model, then take also remote into account
      if ( current == subclassname
          || ( !found_view && current == ossia_pd::o_sym_remote ) )
      {
        object_base* o;
        o = (object_base*)&list->g_pd;
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

bool find_peer(object_base* x)
{
  t_symbol* classname = x->m_obj.o_obj.te_g.g_pd->c_name;
  t_symbol* derived_classname = nullptr;

  if (x->m_otype == object_class::view)
    derived_classname = ossia_pd::o_sym_model;
  else if (x->m_otype == object_class::model)
    derived_classname = ossia_pd::o_sym_view;
  else if (x->m_otype == object_class::device)
    derived_classname = ossia_pd::o_sym_client;
  else if (x->m_otype == object_class::client)
    derived_classname = ossia_pd::o_sym_device;

  // go through all patcher's objects to check there is an incompatibility
  t_gobj* list = x->m_obj.o_canvas->gl_list;
  while (list)
  {
    t_symbol* current = list->g_pd->c_name;
    if (current == classname)
    {
      if (x != (object_base*)&list->g_pd)
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

std::vector<ossia::net::node_base*> find_global_nodes(ossia::string_view addr)
{
  std::vector<ossia::net::node_base*> nodes;
  ossia_pd& instance = ossia_pd::instance();
  size_t pos = addr.find(":");
  if (pos == std::string::npos) return nodes;

  auto prefix = addr.substr(0,pos);
  // remove 'device_name:/' prefix
  auto osc_name = addr.substr(pos+2);

  bool is_prefix_pattern = ossia::traversal::is_pattern(prefix);
  bool is_osc_name_pattern = ossia::traversal::is_pattern(osc_name);
  std::regex pattern(prefix.data(), prefix.size(), std::regex_constants::ECMAScript);

  for (auto device : instance.devices.reference())
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

  for (auto client : instance.clients.reference())
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

std::vector<ossia::value> attribute2value(t_atom* atom, long size)
{
  std::vector<ossia::value> list;
  list.reserve(size);

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
  const ossia::string_view type = s->s_name;

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

// see https://www.reddit.com/r/cpp_questions/comments/6z10d6/getting_that_sweet_zerooverhead_optimization/
std::string replace_brackets(const ossia::string_view sv) {
  std::string str(sv);
  for(std::size_t i = 0, N = str.size(); i < N; i++) {
    switch(str[i])
    {
      case '<':
        str[i] = '{';
        break;
      case '>':
        str[i] = '}';
        break;
      case '|':
        str[i] = ',';
        break;
      default:
        ;
    }
  }
  return str;
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
    error("unknown bounding mode: %s", bounding_mode->s_name);
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

std::vector<ossia::pd::t_matcher*> make_matchers_vector(object_base* x, const ossia::net::node_base* node)
{
  std::vector<ossia::pd::t_matcher*> matchers;
  if (node)
  {
    for (auto& m : x->m_matchers)
    {
      if (node == m.get_node())
      {
        matchers.push_back(&m);
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

void trig_output_value(net::node_base *node)
{
  for(auto param : ossia_pd::instance().params.reference())
  {
    for (auto& m : param->m_matchers)
    {
      if ( m.get_node() == node )
        m.output_value();
    }
  }

  for(auto remote : ossia_pd::instance().remotes.reference())
  {
    for (auto& m : remote->m_matchers)
    {
      if ( m.get_node() == node )
        m.output_value();
    }
  }
}

}
}
