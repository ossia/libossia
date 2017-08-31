// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_obj_base.hpp"
#include "utils.hpp"
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <regex>

#include <ossia/preset/preset.hpp>

extern void glist_noselect(t_glist* x);
extern void canvas_vis(t_canvas* x, t_floatarg f);
extern void canvas_editmode(t_canvas* x, t_floatarg state);
extern void glist_select(t_glist* x, t_gobj* y);

namespace ossia
{
namespace pd
{

#pragma mark t_select_clock

t_select_clock::t_select_clock(t_canvas* cnv, t_object_base* obj) :
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
  x->~t_select_clock();
  ossia_pd::instance().select_clocks.remove_all(x);
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

  if(node)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { enqueue_value(v); });

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

  if(node)
  {
    if(auto param = node->get_parameter())
    {
      if (callbackit)
        param->remove_callback(*callbackit);

      callbackit = param->add_callback(
        [=] (const ossia::value& v) { enqueue_value(v); });

      set_parent_addr();
    }
  }

  return *this;
}

t_matcher::t_matcher(ossia::net::node_base* n, t_object_base* p) :
  node{n}, parent{p}, callbackit{ossia::none}
{
  if (auto param = node->get_parameter())
    callbackit = param->add_callback(
      [=](const ossia::value& v) { enqueue_value(v); });

  node->about_to_be_deleted.connect<t_object_base, &t_object_base::is_deleted>(
        parent);

  set_parent_addr();

  //clock_delay(x_regclock, 0);
}

t_matcher::~t_matcher()
{
  if(node)
  {
    if (parent->m_otype == object_class::param)
    {
      if (!parent->m_is_deleted)
      {
        if (node->get_parent())
          node->get_parent()->remove_child(*node);
      }
      // if there vector is empty
      // remote should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        obj_quarantining<t_param>((t_param*) parent);
      }
    } else {
      auto param = node->get_parameter();
      if (param && callbackit) param->remove_callback(*callbackit);
      node->about_to_be_deleted.disconnect<t_object_base, &t_object_base::is_deleted>(parent);

      // if there vector is empty
      // remote should be quarantinized
      if (parent->m_matchers.size() == 0)
      {
        obj_quarantining<t_remote>((t_remote*) parent);
      }
    }
  }
  node = nullptr;
}

void t_matcher::enqueue_value(const ossia::value& v)
{
  m_queue_list.enqueue(v);
}

void t_matcher::output_value()
{
  ossia::value val;
  while(m_queue_list.try_dequeue(val)) {

    outlet_anything(parent->m_dumpout,gensym("address"),1,&m_addr);

    auto param = node->get_parameter();

    auto filtered = ossia::net::filter_value(
          param->get_domain(),
          val,
          param->get_bounding());

    ossia::value converted;
    if ( parent->m_ounit != ossia::none )
      converted = ossia::convert(filtered, param->get_unit(), *parent->m_ounit);
    else
      converted = filtered;

    value_visitor<t_object_base> vm;
    vm.x = (t_object_base*)parent;
    converted.apply(vm);
  }
}

void t_matcher::set_parent_addr()
{
  if (parent->m_parent_node){
    std::string addr = ossia::net::address_string_from_node(*node);
    std::string parent_addr = ossia::net::address_string_from_node(*parent->m_parent_node);
    if ( parent_addr.back() != '/' ) parent_addr += "/";

    std::regex addr_regex(parent_addr);
    std::smatch addr_match;

    if (std::regex_search(addr, addr_match, addr_regex))
    {
      SETSYMBOL(&m_addr, gensym(addr_match.suffix().str().c_str()));
    } else {
      SETSYMBOL(&m_addr, gensym(node->get_name().c_str()));
    }
  }
  else
  {
    SETSYMBOL(&m_addr, gensym("."));
  }
}

#pragma mark t_obj_base

t_object_base::t_object_base(t_eclass* c)
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

void t_object_base::is_deleted(const ossia::net::node_base& n)
{
  if (!m_dead)
  {
    m_is_deleted= true;
    ossia::remove_one_if(
      m_matchers,
      [&] (const auto& m) {
        return m.get_node() == &n;
    });
    m_is_deleted = false;
  }
}

/**
 * @brief t_obj_base::push : push a value to a node
 * @param x : caller that holds the node to push to
 * @param argc : number of value in the list
 * @param argv :  list of t_atom value(s)
 */
void t_object_base::push(t_object_base* x, t_symbol* s, int argc, t_atom* argv)
{
  ossia::net::node_base* node;

  if (!x->m_mute)
  {
    for (auto& m : x->m_matchers)
    {
      node = m.get_node();
      auto parent = m.get_parent();
      auto param = node->get_parameter();

      if (node && param)
      {
        if (argc == 1)
        {
          ossia::value v;
          // convert one element array to single element
          if (argv->a_type == A_SYMBOL)
            v = ossia::value(std::string(atom_getsymbol(argv)->s_name));
          else if (argv->a_type == A_FLOAT)
            v = ossia::value(atom_getfloat(argv));

          ossia::value vv;

          if ( parent->m_ounit != ossia::none )
          {
            auto src_unit = *parent->m_ounit;
            auto dst_unit = param->get_unit();

            vv = ossia::convert(v, src_unit, dst_unit);
          } else
            vv = v;

          node->get_parameter()->push_value(vv);
        }
        else
        {
          std::vector<ossia::value> list;

          if ( s && s != gensym("list") )
            list.push_back(std::string(s->s_name));

          for (; argc > 0; argc--, argv++)
          {
            if (argv->a_type == A_SYMBOL)
              list.push_back(std::string(atom_getsymbol(argv)->s_name));
            else if (argv->a_type == A_FLOAT)
              list.push_back(atom_getfloat(argv));
            else
              pd_error(x, "value type not handled");
          }
          auto src_unit = *parent->m_ounit;
          auto dst_unit = param->get_unit();

          ossia::convert(list, src_unit, dst_unit);

          node->get_parameter()->push_value(list);
        }
      }
    }
  }
}

/**
 * @brief t_obj_base::bang send out the current value of the parameter
 * @param x
 */
void t_object_base::bang(t_object_base* x)
{
  for (auto& matcher : x->m_matchers)
  {
    matcher.enqueue_value(matcher.get_node()->get_parameter()->value());
    matcher.output_value();
  }
}

void t_object_base::output_value(t_object_base* x)
{
  for (auto& m : x->m_matchers)
  {
    m.output_value();
  }
  clock_set(x->m_poll_clock, x->m_poll_interval);
}

/**
 * @brief list_all_child : list all node childs recursively
 * @param node : starting point
 * @param list : reference to a node_base vector to store each node
 */
void list_all_child(const ossia::net::node_base& node, std::vector<ossia::net::node_base*>& list){
  for (const auto& child : node.children_copy())
  {
    list.push_back(child);
    list_all_child(*child,list);
  }
}

void obj_namespace(t_object_base* x)
{
  t_symbol* prependsym = gensym("namespace");
  std::vector<ossia::net::node_base*> list;
  for (auto n : x->m_nodes)
  {
    list_all_child(*n, list);
    int pos = ossia::net::osc_parameter_string(*n).length();
    for (ossia::net::node_base* child : list)
    {
      if (child->get_parameter())
      {
        ossia::value name = ossia::net::osc_parameter_string(*child).substr(pos);
        ossia::value val = child->get_parameter()->fetch_value();

        std::vector<t_atom> va;
        value2atom vm{va};

        name.apply(vm);
        val.apply(vm);

        outlet_anything(x->m_dumpout, prependsym, va.size(), va.data());
      }
    }
  }
}

void obj_set(t_object_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (argc > 0 && argv[0].a_type == A_SYMBOL)
  {
    std::string addr = argv[0].a_w.w_symbol->s_name;
    argv++;
    argc--;
    for (auto n : x->m_nodes)
    {
      auto nodes = ossia::net::find_nodes(*n, addr);
      for (auto& no : nodes)
      {
        if (no->get_parameter()){
          t_matcher matcher{no,x};
          x->m_matchers.push_back(std::move(matcher));
        }
      }
      t_object_base::push(x,nullptr, argc, argv);
      x->m_matchers.clear();
    }
  }
}

void obj_get_address(t_object_base *x)
{
  if (!x->m_matchers.empty())
  {
    t_symbol* sym_address = gensym("global_address");
    for (auto& m : x->m_matchers)
    {
      std::string addr = ossia::net::address_string_from_node(*m.get_node());
      t_atom a;
      SETSYMBOL(&a, gensym(addr.c_str()));
      outlet_anything(x->m_dumpout, sym_address, 1, &a);
    }
  }
  else if (!x->m_nodes.empty())
  {
    for (auto n : x->m_nodes)
    {
      std::string addr = ossia::net::address_string_from_node(*n);
      t_atom a;
      SETSYMBOL(&a, gensym(addr.c_str()));
      outlet_anything(x->m_dumpout, gensym("address"), 1, &a);
    }
  }
  else
    outlet_anything(x->m_dumpout, gensym("address"), 0, NULL);
}

bool find_and_display_friend(t_object_base* x)
{
  int found = 0;
  if (x->m_otype == object_class::remote)
  {
    for (auto& rm_matcher : x->m_matchers)
    {
      for (auto param : ossia_pd::instance().params.copy())
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
              t_object_base* obj = pa_matcher.get_parent();
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
      for (auto model : ossia_pd::instance().models.copy())
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
              t_object_base* obj = md_matcher.get_parent();
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


void obj_preset(t_object_base *x, t_symbol*s, int argc, t_atom* argv)
{
  ossia::net::node_base* node{};
  switch (x->m_otype)
  {
    case object_class::client:
    case object_class::device:
      node = &x->m_device->get_root_node();
      break;
    case object_class::model:
    case object_class::view:
      // TODO oups how to get that ?
      node = x->m_nodes[0];
      break;
    default:
      node = nullptr;
  }

  t_atom status[3];
  status[0] = argv[0];

  if ( argc < 2
       || argv[0].a_type != A_SYMBOL
       || argv[1].a_type != A_SYMBOL )
  {
    pd_error(x, "Wrong argument number to 'preset' message"
                "needs 2 symbol arguments: <load|save> <filename>");
    return;
  }

  if (node)
  {
    if ( argv[0].a_w.w_symbol == gensym("save") )
    {
      argc--;
      argv++;

      SETFLOAT(status+1, 0);
      status[2] = argv[0];

      try {

        auto preset = ossia::presets::make_preset(*node);
        auto json = ossia::presets::write_json(x->m_name->s_name, preset);
        ossia::presets::write_file(json, argv[0].a_w.w_symbol->s_name);
        SETFLOAT(status+1, 1);

      } catch (std::ifstream::failure e) {
        pd_error(x,"Can't open file %s, error: %s", argv[0].a_w.w_symbol->s_name, e.what());
      }

      outlet_anything(x->m_dumpout, gensym("preset"),3, status);

    }
    else if ( argv[0].a_w.w_symbol == gensym("load") )
    {
      argc--;
      argv++;

      SETFLOAT(status+1, 0);
      status[2] = argv[0];
      try {

        auto json = ossia::presets::read_file(argv[0].a_w.w_symbol->s_name);
        auto preset = ossia::presets::read_json(json);
        ossia::presets::apply_preset(*node, preset);
        SETFLOAT(status+1, 1);

      } catch (std::ifstream::failure e) {

        pd_error(x,"Can't write file %s, error: %s", argv[0].a_w.w_symbol->s_name, e.what());

      }

      outlet_anything(x->m_dumpout, gensym("preset"),3, status);

    }
    else
      pd_error(x, "unknown preset command '%s'", argv[0].a_w.w_symbol->s_name);

  }
  else
  {
    pd_error(x, "No node to save or to load into.");
  }
}

}
} // namespace
