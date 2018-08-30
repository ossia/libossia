// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/object_base.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/preset/preset.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/utils.hpp>
#include <regex>

namespace ossia
{
namespace max
{

#pragma mark t_matcher

t_matcher::t_matcher(t_matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  owner = other.owner;
  other.owner = nullptr;

  callbackit = other.callbackit;
  other.callbackit = ossia::none;

  m_addr = other.m_addr;
  ossia::value v;
  while(other.m_queue_list.try_dequeue(v))
    m_queue_list.enqueue(v);

  m_dead = other.m_dead;

  if(node && !m_dead)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      if (owner)
      {
        callbackit = param->add_callback(
              [=] (const ossia::value& v) { enqueue_value(v); });

        set_parent_addr();
      }
    }
  }
}

t_matcher& t_matcher::operator=(t_matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  owner = other.owner;
  other.owner = nullptr;

  callbackit = other.callbackit;
  other.callbackit = ossia::none;

  ossia::value v;
  while(other.m_queue_list.try_dequeue(v))
    m_queue_list.enqueue(std::move(v));

  m_addr = other.m_addr;

  m_dead = other.m_dead;

  if(node && !m_dead)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      if (owner && !owner->m_is_deleted)
      {
        callbackit = param->add_callback(
              [=] (const ossia::value& v) { enqueue_value(v); });

        set_parent_addr();
      }
    }
  }

  return *this;
}

t_matcher::t_matcher(ossia::net::node_base* n, object_base* p) :
  node{n}, owner{p}, callbackit{ossia::none}
{
  if (owner)
  {
    if (auto param = node->get_parameter())
      callbackit = param->add_callback(
            [=](const ossia::value& v) { enqueue_value(v); });

    node->about_to_be_deleted.connect<&object_base::is_deleted>(owner);
  }

  set_parent_addr();
}

void purge_parent(ossia::net::node_base* node)
{
    // remove parent node recursively if they are no used anymore
  if (auto pn = node->get_parent())
  {
    pn->remove_child(*node);
    if (pn->get_parent() && pn->children().size() == 0)
    {
      bool remove_me = true;
      for (auto model : ossia_max::instance().models.copy())
      {
        for (const auto& m : model->m_matchers)
        {
          if (m->get_node() == pn)
          {
            remove_me = false;
            break;
          }
        }
        if(!remove_me)
          break;
      }
      if (remove_me)
        purge_parent(pn);
    }
  }
}

t_matcher::~t_matcher()
{
  if(node && owner)
  {

    // purge selection
    ossia::remove_one(owner->m_node_selection,this);

    if (   owner->m_otype == object_class::param
        || owner->m_otype == object_class::model )
    {
      if (!owner->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<&object_base::is_deleted>(owner);

        for (auto remote : ossia_max::instance().remotes.copy())
        {
          ossia::remove_one_if(remote->m_matchers, [this] (auto& other) { return *other == *this; });
        }

        for (auto attribute : ossia_max::instance().attributes.copy())
        {
          ossia::remove_one_if(attribute->m_matchers, [this] (auto& other) { return *other == *this; });
        }

        purge_parent(node);
      }
      // if the vector is empty
      // remote should be quarantinized
      if (owner->m_matchers.size() == 0)
      {
        switch(owner->m_otype)
        {
          case object_class::model:
            object_quarantining<model>((model*) owner);
            break;
          case object_class::param:
            object_quarantining<parameter>((parameter*) owner);
            break;
          default:
            ;
        }
      }
    } else {

      if (!owner->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        std::cout << "DISconnect node " << static_cast<void*>(node)
                  << " to is_deleted fn of " << static_cast<void*>(owner) << std::endl;
        node->about_to_be_deleted.disconnect<&object_base::is_deleted>(owner);
      }

      // if there vector is empty
      // remote should be quarantinized
      if (owner->m_matchers.size() == 0)
      {
        switch(owner->m_otype)
        {
          case object_class::attribute:
            object_quarantining<attribute>((attribute*) owner);
            break;
          case object_class::remote:
            object_quarantining<remote>((remote*) owner);
            break;
          case object_class::view:
            object_quarantining<view>((view*) owner);
            break;
          default:
            ;
        }
      }
    }
    node = nullptr;
    owner = nullptr;
  }
}

void t_matcher::enqueue_value(ossia::value v)
{
  auto param = node->get_parameter();
  auto filtered = ossia::net::filter_value(
        param->get_domain(),
        std::move(v),
        param->get_bounding());

  if(!param->filter_value(filtered))
  {
    auto x = (parameter_base*) owner;

    if ( x->m_ounit == ossia::none )
    {
      m_queue_list.enqueue(std::move(filtered));
    }
    else
    {
      m_queue_list.enqueue(ossia::convert(std::move(filtered), param->get_unit(), *x->m_ounit));
    }
    if(m_queue_list.size_approx() > owner->m_queue_length)
    {
      ossia::value val;
      m_queue_list.try_dequeue(val);
    }
  }
}

void t_matcher::output_value()
{
  if(owner)
  {
    std::lock_guard<std::mutex> lock(owner->bindMutex);
    ossia::value val;
    while(m_queue_list.try_dequeue(val))
    {
      bool break_flag = false;

      if(   owner->m_otype == object_class::param
         || owner->m_otype == object_class::remote )
      {
        for (const auto& v : m_set_pool)
        {
          if (v == val){
            break_flag = true;
            ossia::remove_one(m_set_pool, v);
            break;
          }
        }
      }

      if( break_flag )
        continue;

      if(owner->m_dumpout)
        outlet_anything(owner->m_dumpout,gensym("address"),1,&m_addr);

      value_visitor<object_base> vm;
      vm.x = (object_base*)owner;
      val.apply(vm);
    }
  }
}

void t_matcher::set_parent_addr()
{
  if (!m_dead && node && owner && owner->m_parent_node){
    // TODO how to deal with multiple parents ?
    std::string addr = ossia::net::relative_address_string_from_nodes(*node, *owner->m_parent_node);
    A_SETSYM(&m_addr, gensym(addr.c_str()));
  }
  else
  {
    A_SETSYM(&m_addr, gensym("."));
  }
}

object_base::object_base()
{

}

void object_base::get_hierarchy()
{
  t_object* patcher = get_patcher(&m_object);

  while (patcher)
  {
    m_patcher_hierarchy.push_back(patcher);
    patcher = jpatcher_get_parentpatcher(patcher);
  }
}

void object_base::loadbang(object_base* x)
{
  // post("loadbang mess");
  if (x->m_reg_clock)
  {
    clock_unset(x->m_reg_clock);
    clock_free((t_object*)x->m_reg_clock);
    x->m_reg_clock = nullptr;
  }

  if (!x->m_patcher_hierarchy.empty())
  {
    auto& map = ossia_max::instance().root_patcher;

    std::pair<ossia_max::RootMap::iterator, bool> res = map.insert(
                std::pair<t_object*,ossia_max::root_descriptor>(x->m_patcher_hierarchy.back(), {} ));

    if (!res.second)
    {
      // key already exists, then increment count
      ossia_max::root_descriptor& desc = (res.first)->second;
      desc.inc();
#if OSSIA_MAX_AUTOREGISTER
      if (!desc.is_loadbanged)
        clock_delay(ossia_max::instance().m_reg_clock,1);
#endif
    }
    else
    {
#if OSSIA_MAX_AUTOREGISTER
      clock_delay(ossia_max::instance().m_reg_clock,1);
#endif
    }
  }
}

void object_base::is_deleted(const ossia::net::node_base& n)
{
    m_is_deleted = true;
    for(auto nd : m_node_selection)
    {
        if(nd->get_node() == &n)
            nd->set_dead();
    }
    for(auto& nd : m_matchers)
    {
        if(nd->get_node() == &n)
            nd->set_dead();
    }
    m_node_selection.erase(ossia::remove_if(
      m_node_selection,
      [&] (const t_matcher* m) {
        return m->get_node() == &n;
    }), m_node_selection.end());
    m_matchers.erase(ossia::remove_if(
      m_matchers,
      [&] (const std::shared_ptr<t_matcher>& m) {
        return m->get_node() == &n;
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
  for (t_matcher* m : m_node_selection)
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

t_max_err object_base::notify(object_base *x, t_symbol *s,
                       t_symbol *msg, void *sender, void *data)
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
    else if ( s == gensym("recall_safe") )
      x->set_recall_safe();
  }
  return 0;
}

void object_base::get_recall_safe(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    t_atom a;

    A_SETLONG(&a, ossia::net::get_recall_safe(*m->get_node()));
    outlet_anything(x->m_dumpout, gensym("recall_safe"), 1, &a);
  }
  lock_and_touch(x, gensym("recall_safe"));
}

void object_base::get_tags(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

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

      outlet_anything(x->m_dumpout, gensym("tags"),
                      x->m_tags_size, asym);
    }
  }
  lock_and_touch(x, gensym("tags"));
}

void object_base::get_description(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    auto description = ossia::net::get_description(*m->get_node());

    if (description)
    {
      x->m_description = gensym((*description).c_str());
      x->m_description_size = 1;
      t_atom adesc;
      A_SETSYM(&adesc,x->m_description);

      outlet_anything(x->m_dumpout, gensym("description"),
                      1, &adesc);
    } else {
      outlet_anything(x->m_dumpout, gensym("description"), 0, nullptr);
    }
  }
  lock_and_touch(x, gensym("description"));
}

void object_base::get_priority(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    auto priority = ossia::net::get_priority(*m->get_node());
    if (priority)
      x->m_priority = *priority;
    else
      x->m_priority = 0;

    t_atom a;
    A_SETFLOAT(&a, x->m_priority);
    outlet_anything(x->m_dumpout, gensym("priority"), 1, &a);
  }
  lock_and_touch(x, gensym("priority"));
}

void object_base::get_hidden(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    t_atom a;
    A_SETFLOAT(&a, ossia::net::get_hidden(*m->get_node()));
    outlet_anything(x->m_dumpout, gensym("invisible"), 1, &a);
  }
  lock_and_touch(x, gensym("invisible"));
}

void object_base::get_zombie(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    t_atom a;
    A_SETFLOAT(&a, ossia::net::get_zombie(*m->get_node()));
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
    object_post((t_object*)x,"nsso attribute %s", s->s_name);
}

void object_base::get_address(object_base *x, std::vector<t_matcher*> nodes)
{

  t_symbol* sym_address = gensym("global_address");
  for (auto m : nodes)
  {
    std::string addr = ossia::net::address_string_from_node(*m->get_node());
    t_atom a;
    A_SETSYM(&a, gensym(addr.c_str()));
    outlet_anything(x->m_dumpout, sym_address, 1, &a);
  }

  if (nodes.empty())
    outlet_anything(x->m_dumpout, gensym("address"), 0, NULL);
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
    x->m_selection_path = ossia::none;
  }

  x->fill_selection();
}

void object_base::lock_and_touch(object_base* x, t_symbol* s)
{
  x->m_lock = true;
  object_attr_touch((t_object*)x, s);
  x->m_lock = false;
}

void object_base::update_path()
{
    std::string name = object_path_absolute(this);

    m_is_pattern = ossia::traversal::is_pattern(name);

    if(m_is_pattern)
    {
        m_path = ossia::traversal::make_path(name);
    }
    else
    {
        m_path = ossia::none;
    }
}

} // max namespace
} // ossia namespace
