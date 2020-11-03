// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/object_base.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/network/value/value_conversion.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>
#include <regex>

namespace ossia
{
namespace max
{

ossia::safe_set<ossia::net::parameter_base*> object_base::param_locks;

object_base::object_base()
{
  m_patcher = ossia::max::get_patcher(&m_object);
}

object_base::~object_base()
{
  if(m_highlight_clock)
  {
    clock_unset(m_highlight_clock);
    object_free(m_highlight_clock);
  }
  auto& root_map = ossia_max::instance().root_patcher;
  const auto& it = root_map.find(m_patcher_hierarchy.back());

  if(it != root_map.end())
  {
    it->second.dec();
    if(it->second.count == 0)
    {
        root_map.erase(it);
    }
  }
}

void object_base::get_hierarchy()
{
  t_object* patcher = m_patcher;

  auto& vec = m_patcher_hierarchy;
  vec.clear();
  vec.reserve(32);

  while (patcher)
  {
    vec.push_back(patcher);
    patcher = ossia::max::get_patcher(patcher);
  }

  // remove duplicates
  vec.erase( std::unique( vec.begin(), vec.end() ), vec.end() );
}

void object_base::highlight()
{
  if(m_highlight_clock)
    return;

  t_object *box{};

  // get the object's wrapping box
  t_max_err err = object_obex_lookup(this, gensym("#B"), (t_object **)&box);
  if (err != MAX_ERR_NONE)
    return;

  err = jbox_get_color(box, &m_color);
  if(err != MAX_ERR_NONE)
    return;

  // invert the color
  t_jrgba new_color;
  new_color.alpha = 1.0;
  new_color.red   = 1.0;
  new_color.green = 0.0;
  new_color.blue  = 0.0;

  jbox_set_color(box, &new_color);

  m_highlight_clock = clock_new(this, (method) object_base::reset_color);
  clock_delay(m_highlight_clock, 2000);
}

void object_base::reset_color(object_base* x)
{
  t_object *box{};

  // get the object's wrapping box
  t_max_err err = object_obex_lookup(x, gensym("#B"), (t_object **)&box);
  if (err != MAX_ERR_NONE)
    return;

  jbox_set_color(box, &x->m_color);
  object_free(x->m_highlight_clock);
  x->m_highlight_clock=nullptr;
}

void object_base::loadbang(object_base* x)
{
  if(x->m_registered)
    return;

  t_object* patcher = x->m_patcher;
  t_object* root_patcher = patcher;

  while(patcher && !ossia_max::instance().patchers[patcher].loadbanged)
  {
    root_patcher = patcher;
    patcher = get_patcher(patcher);
  }

  if(ossia_max::instance().patchers[root_patcher].loadbanged)
  {
    auto matchers = x->find_parent_nodes();

    if(!matchers.empty())
    {
      switch(x->m_otype)
      {
        case object_class::param:
          static_cast<parameter*>(x)->do_registration(matchers);
          break;
        case object_class::remote:
          static_cast<remote*>(x)->do_registration(matchers);
          break;
        case object_class::attribute:
          static_cast<attribute*>(x)->do_registration(matchers);
          break;
        case object_class::model:
        {
          auto obj = static_cast<model*>(x);
          obj->do_registration(matchers);
          register_children_in_patcher_recursively(root_patcher, obj, obj->m_matchers);
          break;
        }
        case object_class::view:
        {
          auto obj = static_cast<view*>(x);
          obj->do_registration(matchers);
          register_children_in_patcher_recursively(root_patcher, obj, obj->m_matchers);
          break;
        }
        default:
          break;
      }
    }
  }
  else
  {
    register_children_in_patcher_recursively(root_patcher, nullptr,
                                           {std::make_shared<matcher>(&ossia_max::instance().get_default_device()->get_root_node(), nullptr)});
    ossia_max::instance().patchers[root_patcher].loadbanged = true;
  }
}

void object_base::is_deleted(const ossia::net::node_base& n)
{
    m_is_deleted = true;
    for(auto nd : m_node_selection)
    {
      if(nd->get_node() == &n)
      {
        if(nd->is_locked())
          nd->set_zombie();
        nd->set_dead();
      }
    }
    for(auto& nd : m_matchers)
    {
      if(nd->get_node() == &n)
      {
        if(nd->is_locked())
          nd->set_zombie();
        nd->set_dead();
      }
    }
    m_node_selection.erase(ossia::remove_if(
      m_node_selection,
      [&] (const matcher* m) {
        return (m->get_node() == &n) && !m->is_locked();
    }), m_node_selection.end());
    m_matchers.erase(ossia::remove_if(
      m_matchers,
      [&] (const std::shared_ptr<matcher>& m) {
        return (m->get_node() == &n) && !m->is_locked();
    }), m_matchers.end());
    m_is_deleted = false;
}

void object_base::set_priority()
{
  for (auto m : m_node_selection)
    ossia::net::set_priority(*m->get_node(), m_priority);
}

void object_base::set_description()
{
  if (m_description != gensym(""))
  {
    for (auto m : m_node_selection)
      ossia::net::set_description(*m->get_node(), m_description->s_name);
  }
}

void object_base::set_recall_safe()
{
  for (matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_recall_safe(*node, m_recall_safe);
  }
}

void object_base::set_tags()
{
  std::vector<std::string> tags;
  tags.reserve(m_tags_size);
  for (int i = 0; i < m_tags_size; i++)
    tags.push_back(m_tags[i]->s_name);

  for (auto m : m_node_selection)
    ossia::net::set_tags(*m->get_node(), tags);
}

void object_base::set_hidden()
{
  for (auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_hidden(*node, m_invisible > 0);
  }
}

void object_base::defer_set_output(object_base*x, t_symbol*s ,int argc, t_atom* argv){
  outlet_anything(x->m_set_out, s, argc, argv);
}

void object_base::update_attribute(object_base* x, ossia::string_view attribute, const ossia::net::node_base* node)
{
  auto matchers = make_matchers_vector(x,node);

  if (matchers.empty())
    return;

  if ( attribute == ossia::net::text_priority() ){
    get_priority(x, matchers);
  } else if ( attribute == ossia::net::text_description() ){
    get_description(x, matchers);
  } else if ( attribute == ossia::net::text_tags() ){
    get_tags(x, matchers);
  } else if ( attribute == ossia::net::text_hidden() ){
    get_hidden(x, matchers);
  } else if ( attribute == ossia::net::text_recall_safe() ){
    get_recall_safe(x, matchers);
  } else {
    object_error((t_object*)x, "no attribute %s", std::string(attribute).c_str());
  }
}

t_max_err object_base::notify(object_base *x, t_symbol*,
                       t_symbol *msg, void*, void *data)
{
  t_symbol *attrname;

  if (!x->m_lock && msg == gensym("attr_modified")) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if ( attrname == gensym("invisible") )
      x->set_hidden();
    else if ( attrname == gensym("priority") )
      x->set_priority();
    else if ( attrname == gensym("description") )
      x->set_description();
    else if ( attrname == gensym("tags") )
      x->set_tags();
    else if ( attrname == gensym("recall_safe") )
      x->set_recall_safe();
  }
  return 0;
}

void object_base::get_recall_safe(object_base*x, std::vector<matcher*> nodes)
{
  for (auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    t_atom a;

    A_SETLONG(&a, ossia::net::get_recall_safe(*m->get_node()));
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
    outlet_anything(x->m_dumpout, gensym("recall_safe"), 1, &a);
  }
  lock_and_touch(x, gensym("recall_safe"));
}

void object_base::get_tags(object_base*x, std::vector<matcher*> nodes)
{
  for (auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    auto optags = ossia::net::get_tags(*m->get_node());

    if (optags)
    {
      const std::vector<std::string>& tags = *optags;
      x->m_tags_size = tags.size() > OSSIA_MAX_MAX_ATTR_SIZE ? OSSIA_MAX_MAX_ATTR_SIZE : tags.size();
      t_atom asym[OSSIA_MAX_MAX_ATTR_SIZE];
      for (int i=0; i < x->m_tags_size; i++)
      {
        x->m_tags[i] = gensym(tags[i].c_str());
        A_SETSYM(&asym[i], x->m_tags[i]);
      }

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("tags"),
                      x->m_tags_size, asym);
    }
  }
  lock_and_touch(x, gensym("tags"));
}

void object_base::get_description(object_base*x, std::vector<matcher*> nodes)
{
  for (auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    auto description = ossia::net::get_description(*m->get_node());

    if (description)
    {
      x->m_description = gensym((*description).c_str());
      x->m_description_size = 1;
      t_atom adesc;
      A_SETSYM(&adesc,x->m_description);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("description"),
                      1, &adesc);
    } else {
      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("description"), 0, nullptr);
    }
  }
  lock_and_touch(x, gensym("description"));
}

void object_base::get_priority(object_base*x, std::vector<matcher*> nodes)
{
  for (auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    auto priority = ossia::net::get_priority(*m->get_node());
    if (priority)
      x->m_priority = *priority;
    else
      x->m_priority = 0;

    t_atom a;
    A_SETFLOAT(&a, x->m_priority);

    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
    outlet_anything(x->m_dumpout, gensym("priority"), 1, &a);
  }
  lock_and_touch(x, gensym("priority"));
}

void object_base::get_hidden(object_base*x, std::vector<matcher*> nodes)
{
  for (auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    t_atom a;
    A_SETFLOAT(&a, ossia::net::get_hidden(*m->get_node()));
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
    outlet_anything(x->m_dumpout, gensym("invisible"), 1, &a);
  }
  lock_and_touch(x, gensym("invisible"));
}

void object_base::get_zombie(object_base*x, std::vector<matcher*> nodes)
{
  for (auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    t_atom a;
    A_SETFLOAT(&a, ossia::net::get_zombie(*m->get_node()));
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
    outlet_anything(x->m_dumpout, gensym("zombie"), 1, &a);
  }
}

void object_base::class_setup(t_class*c)
{
  CLASS_ATTR_LONG(c, "priority", 0, object_base, m_priority);
  CLASS_ATTR_LABEL(c, "priority", 0, "Priority");

  CLASS_ATTR_SYM(c, "description", 0, object_base, m_description);
  CLASS_ATTR_LABEL(c, "description", 0, "Description");

  CLASS_ATTR_SYM_VARSIZE(c, "tags", 0, object_base, m_tags, m_tags_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "tags", 0, "Tags");

  CLASS_ATTR_LONG( c, "invisible", 0, object_base, m_invisible);
  CLASS_ATTR_STYLE(c, "invisible", 0, "onoff");
  CLASS_ATTR_LABEL(c, "invisible", 0, "Invisible");

  CLASS_ATTR_LONG(c, "recall_safe", 0, object_base, m_recall_safe);
  CLASS_ATTR_STYLE(c, "recall_safe", 0, "onoff");
  CLASS_ATTR_LABEL(c, "recall_safe", 0, "Recall safe");

  class_addmethod(c, (method) object_base::select_mess_cb,  "select",   A_GIMME, 0);
  class_addmethod(c, (method) object_base::select_mess_cb,  "unselect", A_GIMME, 0);
  class_addmethod(c, (method) object_base::loadbang,        "loadbang", A_CANT,  0);
}

void object_base::fill_selection()
{
  m_node_selection.clear();
  if ( m_selection_path )
  {
    for (auto& m : m_matchers)
    {
      if ( ossia::traversal::match(*m_selection_path, *m->get_node()) )
        m_node_selection.push_back(m.get());
    }
  } else {
    for (auto& m : m_matchers)
    {
      m_node_selection.push_back(m.get());
    }
  }
}

void object_base::get_mess_cb(object_base* x, t_symbol* s){
  if (s == gensym("address"))
    get_address(x,x->m_node_selection);
  else if (s == gensym("tags"))
    get_tags(x,x->m_node_selection);
  else if (s == gensym("description"))
    get_description(x,x->m_node_selection);
  else if (s == gensym("invisible"))
    get_hidden(x,x->m_node_selection);
  else if (s == gensym("zombie"))
    get_zombie(x,x->m_node_selection);
  else if (s == gensym("priority"))
    get_priority(x,x->m_node_selection);
  else if (s == gensym("recall_safe"))
    get_recall_safe(x,x->m_node_selection);
  else
    object_post((t_object*)x,"no attribute %s", s->s_name);
}

void object_base::get_address(object_base *x, std::vector<matcher*> nodes)
{
  for (auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    std::string addr = ossia::net::address_string_from_node(*m->get_node());
    t_atom a;
    A_SETSYM(&a, gensym(addr.c_str()));
    outlet_anything(x->m_dumpout, gensym("global_address"), 1, &a);
  }

  if (nodes.empty())
    outlet_anything(x->m_dumpout, gensym("global_address"), 0, NULL);
}

void object_base::select_mess_cb(object_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if ( s == gensym("select")
       && argc
       && argv[0].a_type == A_SYM )
  {
    x->m_selection_path  = ossia::traversal::make_path(atom_getsym(argv)->s_name);
  }
  else
  {
    x->m_selection_path = std::nullopt;
  }

  x->fill_selection();
}

void object_base::lock_and_touch(object_base* x, t_symbol* s)
{
  x->m_lock = true;
  object_attr_touch((t_object*)x, s);
  x->m_lock = false;
}

void object_base::push_parameter_value(ossia::net::parameter_base* param, const ossia::value& val, bool set_flag)
{
  if(!param_locks.contains(param))
  {
    param_locks.push_back(param);
    if(set_flag)
      m_set_pool.push_back(ossia::convert(val, param->get_value_type()));

    param->push_value(val);
    param_locks.remove_all(param);
  }
}

object_base* object_base::find_parent_object()
{
  switch(m_otype)
  {
    case object_class::device:
    case object_class::client:
      return nullptr;
    default:
        ;
  }

  bool look_for_model_view = true;
  switch(m_addr_scope)
  {
    case ossia::net::address_scope::absolute:
      look_for_model_view = false;
    case ossia::net::address_scope::relative:
    {
      switch(m_otype)
      {
        case object_class::model:
        case object_class::view:
          look_for_model_view = false;
          break;
        default:
          break;
      }
      return find_parent_object_recursively(m_patcher, look_for_model_view);
    }
    case ossia::net::address_scope::global:
    {
      return nullptr;
    }
  }
}

object_base* object_base::find_parent_object_recursively(
    t_object* patcher, bool look_for_model_view)
{

  object_base* candidate{};
  // TODO : to avoid iterating over all objects in patcher (which could be very long)
  // we could keep a list of objects in a given patcher and then query that database instead
  // with std::map<t_object* patcher, ossia_objects> where ossia_objects is a structured list of all ossia object in the patcher
  while(patcher)
  {
    // 1: look for device, client, model and view objects into the patcher
    t_object* next_box = object_attr_getobj(patcher, _sym_firstobject);
    while (next_box)
    {
      object_base* object = (object_base*) jbox_get_object(next_box);

      t_symbol* curr_classname = object_attr_getsym(next_box, _sym_maxclass);

      if(look_for_model_view)
      {
        if ( curr_classname == gensym("ossia.model")
            || curr_classname == gensym("ossia.view"))
        {
          return object;
        }
      }

      // if there is a client or device in the current patcher
      // return only that object
      if ( curr_classname == gensym("ossia.device")
          || curr_classname == gensym("ossia.client"))
      {
        // TODO should we check for that ?
        // if yes, then do it always, if no, forget it for ever
        // ignore dying object
        if (!object->m_dead)
        {
          candidate = object;
        }
      }

      next_box = object_attr_getobj(next_box, _sym_nextobject);
    }

    if(candidate)
    {
      return candidate;
    }

    // look into parent patcher
    patcher = ossia::max::get_patcher(patcher);
    look_for_model_view = m_addr_scope == ossia::net::address_scope::relative;
  }

  return nullptr;
}

std::vector<std::shared_ptr<matcher>> object_base::find_parent_nodes()
{
  switch(m_otype)
  {
    case object_class::device:
    case object_class::client:
      return {};
      break;
    default:
        ;
  }

  if (m_name == _sym_nothing)
    return {};

  bool look_for_model_view = true;
  switch(m_addr_scope)
  {
    case ossia::net::address_scope::absolute:
      look_for_model_view = false;
    case ossia::net::address_scope::relative:
    {
      switch(m_otype)
      {
        case object_class::model:
        case object_class::view:
          look_for_model_view = false;
          break;
        default:
          break;
      }
      auto parent = find_parent_object_recursively(m_patcher, look_for_model_view);
      if(parent)
      {
        switch(parent->m_otype)
        {
          case object_class::device:
          case object_class::client:
          {
            auto dev = static_cast<device_base*>(parent);
            if(dev->m_device)
            {
              return {std::make_shared<matcher>(&dev->m_device->get_root_node(), parent)};
            }
            else
              return {};
          }
          case object_class::model:
          case object_class::view:
            return parent->m_matchers;
          default:
            return {std::make_shared<matcher>(&ossia_max::instance().get_default_device()->get_root_node(), nullptr)};
        }
      }
      else
      {
        return {std::make_shared<matcher>(&ossia_max::instance().get_default_device()->get_root_node(), nullptr)};
      }
    }
    case ossia::net::address_scope::global:
    {
      auto nodes = find_global_nodes(std::string(m_name->s_name));
      std::vector<std::shared_ptr<matcher>> matchers;
      matchers.reserve(nodes.size());
      for(auto n : nodes)
      {
        matchers.push_back(std::make_shared<matcher>(n, nullptr));
      }
      return matchers;
    }
  }
}

void object_base::update_path()
{
  std::string name;

  switch(m_addr_scope)
  {
    case ossia::net::address_scope::absolute:
    case ossia::net::address_scope::global:
    {
      name = std::string(m_name->s_name);
      break;
    }
    case ossia::net::address_scope::relative:
    {
      name = make_global_pattern();
      break;
    }
  }

  m_is_pattern = ossia::traversal::is_pattern(name);
  m_path = ossia::traversal::make_path(name);
}


std::string object_base::make_global_pattern()
{
  std::vector<std::string> vs;
  vs.reserve(8);

  vs.push_back(m_name->s_name);

  object_base* parent = find_parent_object();
  while(parent)
  {
    vs.push_back(std::string(parent->m_name->s_name));
    parent = find_parent_object();
  }

  fmt::memory_buffer fullpath;
  auto rit = vs.rbegin();
  for (; rit != vs.rend(); ++rit)
  {
    fmt::format_to(fullpath, "/{}", *rit);
  }

  if (vs.empty())
    fmt::format_to(fullpath, "/");

  return std::string(fullpath.data(), fullpath.size());
}

} // max namespace
} // ossia namespace
