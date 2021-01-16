// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/object_base.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/network/value/value_conversion.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <regex>

namespace ossia
{
namespace max
{

ossia::safe_set<ossia::net::parameter_base*> object_base::param_locks;

object_base::object_base()
{
  m_patcher = ossia::max::get_patcher(&m_object);
  ossia_max::create_patcher_hierarchy(m_patcher);
}

object_base::~object_base()
{
  if(m_highlight_clock)
  {
    clock_unset(m_highlight_clock);
    object_free(m_highlight_clock);
  }

  auto pat_it = ossia_max::instance().patchers.find(m_patcher);
  if(pat_it != ossia_max::instance().patchers.end())
  {
    auto& pat_desc = pat_it->second;
    switch(m_otype)
    {
      case object_class::attribute:
        pat_desc.attributes.remove_all(
            static_cast<attribute*>(this));
        break;
      case object_class::client:
        if(pat_desc.client == this)
          pat_desc.client = nullptr;
        break;
      case object_class::device:
        if(pat_desc.device == this)
          pat_desc.device = nullptr;
        if(!pat_desc.empty())
        {
          auto parent_object = find_parent_object_recursively(pat_desc.parent_patcher, true);
          std::vector<std::shared_ptr<matcher>> matchers{};
          if(parent_object)
            matchers = parent_object->m_matchers;
          else
            matchers.push_back(std::make_shared<matcher>(&ossia_max::instance().get_default_device()->get_root_node(), nullptr));
          register_children_in_patcher_recursively(m_patcher, nullptr);
          output_all_values(m_patcher, true);
        }
        pat_desc.device = nullptr;
        break;
      case object_class::model:
        if(pat_desc.model == this)
          pat_desc.model = nullptr;
        break;
      case object_class::param:
        pat_desc.parameters.remove_all(
            static_cast<parameter*>(this));
        break;
      case object_class::remote:
        pat_desc.remotes.remove_all(
            static_cast<remote*>(this));
        break;
      case object_class::view:
        if(pat_desc.view == this)
          pat_desc.view = nullptr;
        break;
      default:
          ;
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

std::vector<std::shared_ptr<matcher>> object_base::find_or_create_matchers()
{
  std::string addr = get_path().pattern;
  size_t pos = addr.find(":/");
  if (pos == std::string::npos) return {};

  std::string prefix = addr.substr(0,pos);
  // remove 'device_name:/' prefix
  std::string osc_name = addr.substr(pos+2);

  bool is_prefix_pattern = ossia::traversal::is_pattern(prefix);
  bool is_osc_name_pattern = ossia::traversal::is_pattern(osc_name);
  std::regex pattern(prefix.data(), prefix.size(), std::regex_constants::ECMAScript);

  std::vector<ossia::net::generic_device*> devs = get_all_devices();
  std::vector<std::shared_ptr<matcher>> matchers;

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

    // If we found a matching device, then we look for nodes
    if (match)
    {
      std::vector<ossia::net::node_base*> nodes{};

      if (is_osc_name_pattern)
      {
        auto nodes = ossia::net::find_nodes(dev->get_root_node(), osc_name);
      }
      else
      {
        auto node = ossia::net::find_node(dev->get_root_node(),osc_name);
        if (node)
        {
          nodes.push_back(node);
        }
      }

      // For ossia.parameter:
      //    if we find a node, then we check if it already has a parameter.
      //        If no, return the node and a parameter will be created later
      //        If yes, return a new node (with incremeted suffix)
      //    else we create a new node and a parameter
      // For ossia.model: if we found a node, duplicate it, otherwise create it.
      // For others: if we found a node, return it.
      switch(m_otype)
      {
        case object_class::attribute:
        case object_class::remote:
        {
          matchers.reserve(nodes.size());
          for(auto n : nodes)
          {
            if(auto p = n->get_parameter())
            {
              matchers.push_back(std::make_shared<matcher>(n, this));
            }
          }
        }
        case object_class::view:
        case object_class::explorer:
        case object_class::monitor:
        case object_class::search:
        {
          matchers.reserve(nodes.size());
          for(auto n : nodes)
          {
            matchers.push_back(std::make_shared<matcher>(n, this));
          }
          break;
        }
        case object_class::param:
        {
          if(nodes.empty())
          {
            auto new_nodes = ossia::net::create_nodes(dev->get_root_node(), osc_name);
            matchers.reserve(new_nodes.size());
            for(auto n : new_nodes)
            {
              ossia::try_setup_parameter(static_cast<parameter*>(this)->m_type->s_name, *n);
              matchers.push_back(std::make_shared<matcher>(n, this));
            }
          }
          else
          {
            matchers.reserve(nodes.size());
            for(auto n : nodes)
            {
              if(n->get_parameter())
              {
                n = &ossia::net::create_node(*(n->get_parent()), n->get_name());
              }
              ossia::try_setup_parameter(static_cast<parameter*>(this)->m_type->s_name, *n);
              matchers.push_back(std::make_shared<matcher>(n, this));
            }
          }
          break;
        }
        case object_class::model:
        {
          if(nodes.empty())
          {
            auto new_nodes = ossia::net::create_nodes(dev->get_root_node(), osc_name);
            matchers.reserve(new_nodes.size());
            for(auto n : new_nodes)
            {
              matchers.push_back(std::make_shared<matcher>(n, this));
            }
          }
          else
          {
            matchers.reserve(nodes.size());
            for(auto n : nodes)
            {
              n = &ossia::net::create_node(*(n->get_parent()), n->get_name());
              matchers.push_back(std::make_shared<matcher>(n, this));
            }
          }
          break;
        }
        default:
            ;
      }
    }
  }

  if(matchers.empty())
  {
    switch(m_otype)
    {
      case object_class::param:
      case object_class::model:
      {
        auto name = std::string(prefix);
        object_error(&m_object, "Can't find a device with name: %s", name.c_str());
        break;
      }
      default:
        ;
    }
  }

  return matchers;
}

void object_base::closebang(object_base* x)
{
  x->m_dead = true;
}

void object_base::loadbang(object_base* x)
{
  if(x->m_registered)
    return;

  if(!x->m_name || x->m_name == _sym_nothing)
    return;

  critical_enter(0);
  t_object* patcher = x->m_patcher;
  t_object* root_patcher = patcher;

  if(!x->m_path)
    x->update_path();

  while(patcher && !ossia_max::instance().patchers[patcher].loadbanged)
  {
    root_patcher = patcher;
    patcher = get_patcher(patcher);
  }

  // if patcher has been loadbanged, then register only that object and its children
  if(ossia_max::instance().patchers[root_patcher].loadbanged
  && x->m_otype != object_class::device
  && x->m_otype != object_class::client )
  {
    switch(x->m_otype)
    {
      case object_class::param:
        static_cast<parameter*>(x)->do_registration();
        static_cast<parameter*>(x)->output_values(true);
        break;
      case object_class::remote:
        static_cast<remote*>(x)->do_registration();
        static_cast<remote*>(x)->output_values(false);
        break;
      case object_class::attribute:
        static_cast<attribute*>(x)->do_registration();
        break;
      case object_class::model:
      {
        auto obj = static_cast<model*>(x);
        obj->do_registration();
        register_children_in_patcher_recursively(root_patcher, obj);
        output_all_values(root_patcher, true);
        break;
      }
      case object_class::view:
      {
        auto obj = static_cast<view*>(x);
        obj->do_registration();
        register_children_in_patcher_recursively(root_patcher, obj);
        output_all_values(root_patcher, false);
        break;
      }
      default:
        break;
    }
  }
  else
  {
    // if patcher has not been loadbanged, register all objects in that patcher
    // this happens when the patcher is loaded or instanciated as an abstraction
    // and also when it is pasted / duplicated
    register_children_in_patcher_recursively(root_patcher, nullptr);
    output_all_values(root_patcher, true);
  }
  critical_exit(0);
}

void object_base::on_node_removing(const ossia::net::node_base& n)
{
    m_is_deleted = true;
    // TODO why is it necessary to iterate over m_node_selection AND m_matchers ?
    // the former is a subset of the later
    // isn't it enough to call fill_selection() after ?
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
    else if ( attrname == gensym("trim_addr") )
    {
      for(const auto& m : x->m_matchers)
      {
        m->set_addr_symbol();
      }
    }
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
  CLASS_ATTR_FLOAT(c, "priority", 0, object_base, m_priority);
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

  CLASS_ATTR_LONG(c, "trim_addr", 0, object_base, m_trim_addr);
  CLASS_ATTR_STYLE(c, "trim_addr", 0, "onoff");
  CLASS_ATTR_LABEL(c, "trim_addr", 0, "Trim address reported by dumpout for convenience (default ON)");

  class_addmethod(c, (method) object_base::select_mess_cb,  "select",    A_GIMME, 0);
  class_addmethod(c, (method) object_base::select_mess_cb,  "unselect",  A_GIMME, 0);
  class_addmethod(c, (method) object_base::loadbang,        "loadbang",  A_CANT,  0);
  class_addmethod(c, (method) object_base::closebang,       "closebang", A_CANT,  0);
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

void object_base::push_parameter_value(ossia::net::parameter_base* param, const ossia::value& val)
{
  if(!param_locks.contains(param))
  {
    param_locks.push_back(param);
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
  assert(m_addr_scope != ossia::net::address_scope::global);
  while(patcher)
  {
    auto& pat_desc = ossia_max::instance().patchers[patcher];
    auto vec = std::vector<object_base*>{};
    if(m_addr_scope == ossia::net::address_scope::relative
        && look_for_model_view)
    {
      vec = std::vector<object_base*> {pat_desc.model, pat_desc.view, pat_desc.device, pat_desc.client};
    }
    else
    {
      vec = std::vector<object_base*> {pat_desc.device, pat_desc.client};
    }

    for(auto ptr : vec)
    {
      if(ptr && ptr != this && !ptr->m_dead)
      {
        return ptr;
      }
    }

    patcher = pat_desc.parent_patcher;
  }

  return nullptr;
}

std::vector<std::shared_ptr<matcher>> object_base::find_parent_nodes()
{
  assert(m_addr_scope != ossia::net::address_scope::global);

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
            return parent->m_matchers; // should we duplicate matcher and set owner here to this ???
          default:
            return {std::make_shared<matcher>(&ossia_max::instance().get_default_device()->get_root_node(), this)};
        }
      }
      else
      {
        return {std::make_shared<matcher>(&ossia_max::instance().get_default_device()->get_root_node(), this)};
      }
    }
  }
  return {};
}

void object_base::update_path()
{
  std::string name;

  switch(m_addr_scope)
  {
    case ossia::net::address_scope::absolute:
    {
      std::string dev_name = "*:";
      auto parent = static_cast<device_base*>(find_parent_object());
      if(parent)
      {
        if(parent->m_device)
          dev_name = parent->m_device->get_name();
      }
      else
      {
        dev_name = ossia_max::instance().get_default_device()->get_name();
      }
      name = dev_name + ":" + std::string(m_name->s_name);
      break;
    }
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
  vs.reserve(32);

  assert(m_name);
  vs.push_back(m_name->s_name);

  std::string device_name = ossia_max::instance().get_default_device()->get_name();

  object_base* parent = find_parent_object();
  while(parent)
  {
    // TODO no need to iterate over all parents, take the first one's path and append this m_name to it.
    assert(parent->m_name);
    if(parent->m_otype == object_class::device
    || parent->m_otype == object_class::client)
    {
      auto dev = static_cast<device_base*>(parent);
      if(dev->m_device)
      {
        device_name = dev->m_device->get_name();
        break;
      }
    }
    vs.push_back(parent->m_name->s_name);
    if(parent->m_addr_scope == ossia::net::address_scope::global)
    {
      device_name = "";
      break;
    }
    parent = parent->find_parent_object();
  }

  fmt::memory_buffer absolute_path;


  if(device_name != "")
    fmt::format_to(absolute_path, "{}:", device_name);
  else
  {
    fmt::format_to(absolute_path, "{}", vs.back());
    vs.pop_back();
  }

  auto rit = vs.rbegin();
  for (; rit != vs.rend(); ++rit)
  {
    fmt::format_to(absolute_path, "/{}", *rit);
  }

  if (vs.empty())
    fmt::format_to(absolute_path, "/");

  return std::string(absolute_path.data(), absolute_path.size());
}

void save_children_recursively(t_object* patcher)
{
  auto& pat_desc = ossia_max::instance().patchers[patcher];

  for(auto x : pat_desc.parameters)
  {
    x->save_values();
  }

  for(auto subpatch : pat_desc.subpatchers)
  {
    save_children_recursively(subpatch);
  }
}

void object_base::save_children_state()
{
  switch(m_otype)
  {
    case object_class::model:
      save_children_recursively(m_patcher);
      break;
    case object_class::param:
      static_cast<parameter*>(this)->save_values();
      break;
    default:
        ;
  }
}

} // max namespace
} // ossia namespace
