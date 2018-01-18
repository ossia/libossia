// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-pd/src/object_base.hpp>
#include <ossia-pd/src/utils.hpp>

#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <regex>

extern void glist_noselect(t_glist* x);
extern void canvas_vis(t_canvas* x, t_floatarg f);
extern void canvas_editmode(t_canvas* x, t_floatarg state);
extern void glist_select(t_glist* x, t_gobj* y);

namespace ossia
{
namespace pd
{

#pragma mark t_select_clock

t_select_clock::t_select_clock(t_canvas* cnv, object_base* obj) :
  m_canvas(cnv), m_obj(obj)
{
  m_clock = clock_new(this, (t_method) t_select_clock::deselect);
  ossia_pd::instance().select_clocks.push_back(this);
  clock_delay(m_clock, 1000);
}

t_select_clock::~t_select_clock()
{
  clock_free(m_clock);
}

void t_select_clock::deselect(t_select_clock* x)
{
  glist_deselect(x->m_canvas, (t_gobj*) x->m_obj);
  ossia_pd::instance().select_clocks.remove_all(x);
  delete x;
}

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

  if(node)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { enqueue_value(v); });

      if(owner)
        set_owner_addr();
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

  if(node)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { enqueue_value(v); });

      if(owner)
        set_owner_addr();
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

    node->about_to_be_deleted.connect<object_base,
        &object_base::is_deleted>(owner);
    set_owner_addr();
  }
}

void purge_parent(ossia::net::node_base* node)
{
  // remove parent node.s recursively if they are no used anymore
  if (auto pn = node->get_parent())
  {
    pn->remove_child(*node);
    if (pn->get_parent() && pn->children().size() == 0)
    {
      bool remove_me = true;
      for (auto model : ossia_pd::instance().models.copy())
      {
        for (const auto& m : model->m_matchers)
        {
          if (m.get_node() == pn)
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
        || owner->m_otype == object_class::model  )
    {
      if (!owner->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<object_base, &object_base::is_deleted>(owner);

        for (auto remote : ossia_pd::instance().remotes.copy())
        {
          ossia::remove_one(remote->m_matchers,*this);
        }

        for (auto attribute : ossia_pd::instance().attributes.copy())
        {
          ossia::remove_one(attribute->m_matchers,*this);
        }

        purge_parent(node);
      }
      // if there is no more matcher,
      // object should be quarantinized
      if (owner->m_matchers.size() == 0)
      {
        switch(owner->m_otype)
        {
          case object_class::model:
            obj_quarantining<model>((model*) owner);
            break;
          case object_class::param:
            obj_quarantining<parameter>((parameter*) owner);
            break;
          default:
            ;
        }
      }
    }
    else
    {
      auto param = node->get_parameter();
      if (param && callbackit) param->remove_callback(*callbackit);
      node->about_to_be_deleted.disconnect<object_base, &object_base::is_deleted>(owner);

      // if there is no more matcher,
      // object should be quarantinized
      if (owner->m_matchers.size() == 0)
      {
        switch(owner->m_otype)
        {
          case object_class::remote:
            obj_quarantining<remote>((remote*) owner);
            break;
          case object_class::view:
            obj_quarantining<view>((view*) owner);
            break;
          case object_class::attribute:
            obj_quarantining<attribute>((attribute*) owner);
            break;
          default:
            ;
        }
      }
    }
  }
  node = nullptr;
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
    auto x = (ossia::pd::parameter_base*) owner;

    if ( x->m_ounit == ossia::none )
    {
      m_queue_list.enqueue(std::move(filtered));
    }
    else
    {
      m_queue_list.enqueue(ossia::convert(std::move(filtered), param->get_unit(), *x->m_ounit));
    }
  }
}

void t_matcher::output_value()
{
  ossia::value val;
  while(m_queue_list.try_dequeue(val)) {
    bool break_flag = false;

    for (auto v : m_set_pool)
    {
      if (v == val)
      {
        break_flag = true;
        ossia::remove_one(m_set_pool, v);
        break;
      }
    }

    if( break_flag )
      continue;

    outlet_anything(owner->m_dumpout,gensym("address"),1,&m_addr);

    value_visitor<object_base> vm;
    vm.x = (object_base*)owner;
    val.apply(vm);
  }
}

void t_matcher::set_owner_addr()
{
  if (owner->m_parent_node){
    // TODO how to deal with multiple parents ?
    std::string addr = ossia::net::relative_address_string_from_nodes(*node, *owner->m_parent_node);
    SETSYMBOL(&m_addr, gensym(addr.c_str()));
  }
  else
  {
    SETSYMBOL(&m_addr, gensym("."));
  }
}

#pragma mark t_obj_base

object_base::object_base(t_eclass* c)
{
  char buffer[MAXPDSTRING];

  if (c)
  {
    m_obj.o_obj.te_g.g_pd = (t_pd)c;

    if(c->c_class.c_patchable)
    {
      m_obj.o_obj.te_inlet = 0;
      m_obj.o_obj.te_outlet = 0;
    }

    m_obj.o_nproxy = 0;
    m_obj.o_proxy = NULL;
    m_obj.o_canvas = canvas_getcurrent();

    sprintf(buffer,"#%s%lx", c->c_class.c_name->s_name, (long unsigned int)this);
    m_obj.o_id = gensym(buffer);
    pd_bind(&m_obj.o_obj.ob_pd, m_obj.o_id);
    sprintf(buffer,".x%lx.c", (long unsigned int)m_obj.o_canvas);
    c->c_widget.w_dosave = (t_typ_method)eobj_dosave;
  }
}

void object_base::is_deleted(const ossia::net::node_base& n)
{
  m_is_deleted= true;

  ossia::remove_one_if(
        m_node_selection,
        [&] (const auto& m) {
    return m->get_node() == &n;
  });

  ossia::remove_one_if(
        m_matchers,
        [&] (auto& m) {
    return m.get_node() == &n;
  });

  m_is_deleted = false;
}

void object_base::set_description()
{
  fmt::MemoryWriter description;
  for (int i = 0; i < m_description_size; i++)
  {
    switch(m_description[i].a_type)
    {
      case A_SYMBOL:
        description << m_description[i].a_w.w_symbol->s_name << " ";
        break;
      case A_FLOAT:
        {
          description << m_description[i].a_w.w_float << " ";
          break;
        }
      default:
        ;
    }
  }

  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_description(*node, description.str());
  }
}

void object_base::set_tags()
{
  std::vector<std::string> tags;
  for (int i = 0; i < m_tags_size; i++)
  {
    switch(m_tags[i].a_type)
    {
      case A_SYMBOL:
        tags.push_back(m_tags[i].a_w.w_symbol->s_name);
        break;
      case A_FLOAT:
        {
          std::stringstream ss;
          ss << m_tags[i].a_w.w_float;
          tags.push_back(ss.str());
          break;
        }
      default:
        ;
    }
  }

  for (t_matcher* m : m_node_selection)
    ossia::net::set_tags(*m->get_node(), tags);
}

void object_base::set_priority()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_priority(*node, m_priority);
  }
}

void object_base::set_hidden()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_hidden(*node, m_hidden);
  }
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
      x->m_tags_size = tags.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : tags.size();
      for (int i=0; i < x->m_tags_size; i++)
      {
        SETSYMBOL(&x->m_tags[i], gensym(tags[i].c_str()));
      }

      outlet_anything(x->m_dumpout, gensym("tags"),
                      x->m_tags_size, x->m_tags);
    }
  }
}

void object_base::get_description(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    auto description = ossia::net::get_description(*m->get_node());

    if (description)
    {
      SETSYMBOL(x->m_description, gensym((*description).c_str()));
      x->m_description_size = 1;

      outlet_anything(x->m_dumpout, gensym("description"),
                      x->m_description_size, x->m_description);
    } else {
      outlet_anything(x->m_dumpout, gensym("description"), 0, nullptr);
    }
  }
}

void object_base::get_priority(object_base* x, std::vector<t_matcher*> nodes)
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
    SETFLOAT(&a, x->m_priority);
    outlet_anything(x->m_dumpout, gensym("priority"), 1, &a);
  }
}

void object_base::get_hidden(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    t_atom a;
    SETFLOAT(&a, ossia::net::get_hidden(*m->get_node()));
    outlet_anything(x->m_dumpout, gensym("hidden"), 1, &a);
  }
}

void object_base::get_zombie(object_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    t_atom a;
    SETFLOAT(&a, ossia::net::get_zombie(*m->get_node()));
    outlet_anything(x->m_dumpout, gensym("zombie"), 1, &a);
  }
}

void object_base::fill_selection()
{
  m_node_selection.clear();
  if ( m_selection_path )
  {
    for (auto& m : m_matchers)
    {
      if ( ossia::traversal::match(*m_selection_path, *m.get_node()) )
        m_node_selection.push_back(&m);
    }
  } else {
    for (auto& m : m_matchers)
    {
      m_node_selection.push_back(&m);
    }
  }
}

void object_base::get_address(object_base *x, std::vector<t_matcher*> nodes)
{

  t_symbol* sym_address = gensym("global_address");

  for (auto m : nodes)
  {
    std::string addr = ossia::net::address_string_from_node(*m->get_node());
    t_atom a;
    SETSYMBOL(&a, gensym(addr.c_str()));
    outlet_anything(x->m_dumpout, sym_address, 1, &a);
  }

  if (nodes.empty())
    outlet_anything(x->m_dumpout, gensym("global_address"), 0, NULL);
}

void object_base::select_mess_cb(object_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (argc && argv[0].a_type == A_SYMBOL)
  {
    std::string name = replace_brackets(atom_getsymbol(argv)->s_name);
    x->m_selection_path  = ossia::traversal::make_path(name);
  }
  else
    x->m_selection_path = ossia::none;

  x->fill_selection();
}

void object_base::update_path()
{
  std::string name = get_absolute_path(this);

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

bool ossia::pd::object_base::find_and_display_friend(object_base* x)
{
  int found = 0;
  if (x->m_otype == object_class::remote
      || x->m_otype == object_class::attribute )
  {
    for (auto& rm_matcher : x->m_matchers)
    {
      for (auto param : ossia_pd::instance().params.reference())
      {
        for (auto& pa_matcher : param->m_matchers)
        {
          if ( rm_matcher == pa_matcher )
          {
            found++;

            if (found > 10)
            {
              pd_error(x, "We only display the first 10 connected nodes, and yes this is arbitrary for the sake of your eyes.");
            } else {
              // display it
              object_base* obj = pa_matcher.get_owner();
              t_canvas* patcher = obj->m_obj.o_canvas;

              canvas_vis(glist_getcanvas(patcher), 1);
              glist_select(patcher, (t_gobj*) obj);
            }
          }
        }
      }
    }
  }
  else if (x->m_otype == object_class::view)
  {
    for (auto& vw_matcher : x->m_matchers)
    {
      for (auto model : ossia_pd::instance().models.reference())
      {
        for (auto& md_matcher : model->m_matchers)
        {
          if ( vw_matcher == md_matcher )
          {
            found++;

            if (found > 10)
            {
              pd_error(x, "We only display the first 10 connected nodes, and yes this is arbitrary for the sake of your eyes.");
            } else {
              // display it
              object_base* obj = md_matcher.get_owner();
              t_canvas* patcher = obj->m_obj.o_canvas;

              canvas_vis(glist_getcanvas(patcher), 1);
              glist_select(patcher, (t_gobj*) obj);
            }
          }
        }
      }
    }
  }
  return found;
}

void object_base::class_setup(t_eclass*c)
{
  CLASS_ATTR_INT         (c, "priority",          0, object_base, m_priority);
  CLASS_ATTR_ATOM_VARSIZE(c, "description", 0, object_base, m_description, m_description_size, OSSIA_PD_MAX_ATTR_SIZE);
  CLASS_ATTR_ATOM_VARSIZE(c, "tags", 0, object_base, m_tags, m_tags_size, OSSIA_PD_MAX_ATTR_SIZE);
  CLASS_ATTR_INT         (c, "hidden",            0, object_base, m_hidden);

  eclass_addmethod(c, (method) object_base::select_mess_cb,  "select",    A_GIMME,  0);
  eclass_addmethod(c, (method) object_base::select_mess_cb,  "unselect",  A_NULL,   0);
}

void object_base::get_mess_cb(object_base* x, t_symbol* s){
  if (s == gensym("global_address"))
    get_address(x,x->m_node_selection);
  else if (s == gensym("tags"))
    get_tags(x,x->m_node_selection);
  else if (s == gensym("description"))
    get_description(x,x->m_node_selection);
  else if (s == gensym("hidden"))
    get_hidden(x,x->m_node_selection);
  else if (s == gensym("zombie"))
    get_zombie(x,x->m_node_selection);
  else
    logpost(x,2,"no attribute %s", s->s_name);
}

void object_base::update_attribute(object_base* x, ossia::string_view attribute, const ossia::net::node_base* node)
{
  auto matchers = make_matchers_vector(x,node);

  if ( attribute == ossia::net::text_priority() ){
    get_priority(x, matchers);
  } else if ( attribute == ossia::net::text_description() ){
    get_description(x, matchers);
  } else if ( attribute == ossia::net::text_tags() ){
    get_tags(x, matchers);
  } else if ( attribute == ossia::net::text_hidden() ){
    get_hidden(x, matchers);
  } else {
    logpost(x, 4, "libossia attribute '%s' is not handled in Pd", std::string(attribute).c_str());
  }
}


}
} // namespace
