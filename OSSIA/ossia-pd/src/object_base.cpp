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

  parent = other.parent;
  other.parent = nullptr;

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

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { enqueue_value(v); });

      if(parent)
        set_parent_addr();
    }
  }
}

t_matcher& t_matcher::operator=(t_matcher&& other)
{
  node = other.node;
  other.node = nullptr;

  parent = other.parent;
  other.parent = nullptr;

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

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { enqueue_value(v); });

      if(parent)
        set_parent_addr();
    }
  }

  return *this;
}

t_matcher::t_matcher(ossia::net::node_base* n, object_base* p) :
  node{n}, parent{p}, callbackit{ossia::none}
{
  if (auto param = node->get_parameter())
    callbackit = param->add_callback(
      [=](const ossia::value& v) { enqueue_value(v); });

  if (parent)
  {
    node->about_to_be_deleted.connect<object_base, &object_base::is_deleted>(
          parent);
    set_parent_addr();
  }

  //clock_delay(x_regclock, 0);
}

t_matcher::~t_matcher()
{
  if (m_dead) return;
  if(node && parent)
  {
    if (   parent->m_otype == object_class::param
        || parent->m_otype == object_class::model  )
    {
      if (!parent->m_is_deleted)
      {
        auto param = node->get_parameter();
        if (param && callbackit) param->remove_callback(*callbackit);
        node->about_to_be_deleted.disconnect<object_base, &object_base::is_deleted>(parent);

        for (auto remote : ossia_pd::instance().remotes.copy())
        {
          ossia::remove_one(remote->m_matchers,*this);
          ossia::remove_one(remote->m_node_selection, this);
        }

        for (auto attribute : ossia_pd::instance().attributes.copy())
        {
          ossia::remove_one(attribute->m_matchers,*this);
          ossia::remove_one(attribute->m_node_selection, this);
        }

        if (node->get_parent())
          node->get_parent()->remove_child(*node);
      }
      // if there is no more matcher,
      // object should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        switch(parent->m_otype)
        {
          case object_class::model:
            obj_quarantining<model>((model*) parent);
            break;
          case object_class::param:
            obj_quarantining<parameter>((parameter*) parent);
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
      node->about_to_be_deleted.disconnect<object_base, &object_base::is_deleted>(parent);

      // if there is no more matcher,
      // object should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        switch(parent->m_otype)
        {
          case object_class::remote:
            obj_quarantining<remote>((remote*) parent);
            break;
          case object_class::view:
            obj_quarantining<view>((view*) parent);
            break;
          case object_class::attribute:
            obj_quarantining<attribute>((attribute*) parent);
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

  if(!param->filter_value(v))
  {
    auto x = (ossia::pd::parameter_base*) parent;

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

    outlet_anything(parent->m_dumpout,gensym("address"),1,&m_addr);

    value_visitor<object_base> vm;
    vm.x = (object_base*)parent;
    val.apply(vm);
  }
}

void t_matcher::set_parent_addr()
{
  if (parent->m_parent_node){
    // TODO how to deal with multiple parents ?
    std::string addr = ossia::net::relative_address_string_from_nodes(*node, *parent->m_parent_node);
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

  m_selection_pattern = gensym("*");
}

void object_base::is_deleted(const ossia::net::node_base& n)
{
  if (!m_dead)
  {
    m_is_deleted= true;
    ossia::remove_one_if(
      m_matchers,
      [&] (auto& m) {
        m.set_dead();
        return m.get_node() == &n;
    });
    ossia::remove_one(m_nodes, &n);
    // FIXME purge m_node_selection doesn't work well
    // rebuild it from scratch instead
    /*
    ossia::remove_one_if(
      m_node_selection,
      [&] (auto m) {
        return m->get_node() == &n;
    });
    */
    fill_selection();
    m_is_deleted = false;
  }
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

void object_base::fill_selection()
{
  m_node_selection.clear();
  if ( m_selection_pattern != gensym("*") )
  {
    // TODO should support pattern matching in selection
    for (auto& m : m_matchers)
    {
      if ( m_selection_pattern == m.get_atom_addr_ptr()->a_w.w_symbol )
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
    outlet_anything(x->m_dumpout, gensym("address"), 0, NULL);
}

void object_base::select_mess_cb(object_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (argc && argv[0].a_type == A_SYMBOL)
    x->m_selection_pattern = atom_getsymbol(argv);
  else
    x->m_selection_pattern = gensym("*");

  x->fill_selection();
}

void object_base::update_path()
{
    m_is_pattern = ossia::traversal::is_pattern(m_name->s_name);

    if(m_is_pattern)
    {
        m_path = ossia::traversal::make_path(m_name->s_name);
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
              object_base* obj = pa_matcher.get_parent();
              t_canvas* patcher = obj->m_obj.o_canvas;

              canvas_vis(glist_getcanvas(patcher), 1);
              glist_select(patcher, (t_gobj*) obj);

              // clock will commit suicide after 1 sec.
              t_select_clock* clock = new t_select_clock(patcher, obj);

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
              object_base* obj = md_matcher.get_parent();
              t_canvas* patcher = obj->m_obj.o_canvas;

              canvas_vis(glist_getcanvas(patcher), 1);
              glist_select(patcher, (t_gobj*) obj);

              // clock will commit suicide after 1 sec.
              t_select_clock* clock = new t_select_clock(patcher, obj);

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
  if (s == gensym("address"))
    get_address(x,x->m_node_selection);
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
