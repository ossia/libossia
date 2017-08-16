// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_obj_base.hpp"
#include "utils.hpp"
#include <ossia/network/osc/detail/osc.hpp>

extern void glist_noselect(t_glist* x);
extern void canvas_vis(t_canvas* x, t_floatarg f);
extern void canvas_editmode(t_canvas* x, t_floatarg state);
extern void glist_select(t_glist* x, t_gobj* y);

namespace ossia
{
namespace pd
{

#pragma mark t_select_clock

t_select_clock::t_select_clock(t_canvas* cnv, t_obj_base* obj) :
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
    if(auto addr = node->get_address())
    {
      if (callbackit)
        addr->remove_callback(*callbackit);

      callbackit = addr->add_callback(
        [=] (const ossia::value& v) { set_value(v); });
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
    if(auto addr = node->get_address())
    {
      if (callbackit)
        addr->remove_callback(*callbackit);

      callbackit = addr->add_callback(
        [=] (const ossia::value& v) { set_value(v); });
    }
  }

  return *this;
}

t_matcher::t_matcher(ossia::net::node_base* n, t_obj_base* p) :
  node{n}, parent{p}, callbackit{ossia::none}
{
  callbackit = node->get_address()->add_callback(
      [=](const ossia::value& v) { set_value(v); });

  node->about_to_be_deleted.connect<t_obj_base, &t_obj_base::is_deleted>(
        parent);

  //clock_delay(x_regclock, 0);
}

t_matcher::~t_matcher()
{
  if(node)
  {
    if (parent->x_otype == Type::param)
    {
      if (!parent->x_is_deleted)
      {
        if (node->get_parent())
          node->get_parent()->remove_child(*node);
      }
      // if there vector is empty
      // remote should be quarantinized
      if (parent->x_matchers.size() == 0)
      {
        obj_quarantining<t_param>((t_param*) parent);
      }
    } else {
      auto addr = node->get_address();
      if (addr && callbackit) addr->remove_callback(*callbackit);
      node->about_to_be_deleted.disconnect<t_obj_base, &t_obj_base::is_deleted>(parent);

      // if there vector is empty
      // remote should be quarantinized
      if (parent->x_matchers.size() == 0)
      {
        obj_quarantining<t_remote>((t_remote*) parent);
      }
    }
  }
  node = nullptr;
}

void t_matcher::set_value(const ossia::value& v){
  std::string addr = node->get_name();
  t_atom a;
  SETSYMBOL(&a, gensym(addr.c_str()));
  outlet_anything(parent->x_dumpout,gensym("address"),1,&a);

  value_visitor<t_obj_base> vm;
  vm.x = (t_obj_base*)parent;
  v.apply(vm);
}

#pragma mark t_obj_base

void t_obj_base::setValue(const ossia::value& v)
{
  auto local_address = x_node->get_address();
  auto filtered = ossia::net::filter_value(
        local_address->get_domain(),
        v,
        local_address->get_bounding());
  value_visitor<t_obj_base> vm;
  vm.x = (t_obj_base*)&x_obj;
  filtered.apply(vm);
}

void t_obj_base::is_deleted(const ossia::net::node_base& n)
{
  if (!x_dead)
  {
    x_is_deleted= true;
    ossia::remove_one_if(
      x_matchers,
      [&] (const auto& m) {
        return m.get_node() == &n;
    });
    x_is_deleted = false;
  }
}

/**
 * @brief t_obj_base::obj_push : push a value to a node
 * @param x : caller that holds the node to push to
 * @param argc : number of value in the list
 * @param argv :  list of t_atom value(s)
 */
void t_obj_base::obj_push(t_obj_base* x, t_symbol*, int argc, t_atom* argv)
{
  for (auto& m : x->x_matchers)
  {
    x->x_node = m.get_node();

    if (x->x_node && x->x_node->get_address())
    {
      if (argc == 1)
      {
        // convert one element array to single element
        if (argv->a_type == A_SYMBOL)
          x->x_node->get_address()->push_value(
                std::string(atom_getsymbol(argv)->s_name));
        else if (argv->a_type == A_FLOAT)
          x->x_node->get_address()->push_value(atom_getfloat(argv));
      }
      else
      {
        std::vector<ossia::value> list;
        for (; argc > 0; argc--, argv++)
        {
          if (argv->a_type == A_SYMBOL)
            list.push_back(std::string(atom_getsymbol(argv)->s_name));
          else if (argv->a_type == A_FLOAT)
            list.push_back(atom_getfloat(argv));
          else
            pd_error(x, "value type not handled");
        }
        x->x_node->get_address()->push_value(list);
      }
    }
  }
  x->x_node = nullptr;
}

/**
 * @brief t_obj_base::obj_bang send out the current value of the parameter
 * @param x
 */
void t_obj_base::obj_bang(t_obj_base* x)
{
  if (x->x_node && x->x_node->get_address())
    x->setValue(x->x_node->get_address()->value());
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

/**
 * @brief obj_namespace : send namespace trought dump output
 * @details each message is prepend with "namespace"
 * and adresses start with a '/' to make it each to parse with OSC tool
 * @param x
 */
void obj_namespace(t_obj_base* x)
{
  t_symbol* prependsym = gensym("namespace");
  std::vector<ossia::net::node_base*> list;
  list_all_child(*x->x_node, list);
  int pos = ossia::net::osc_address_string(*x->x_node).length();
  for (ossia::net::node_base* child : list)
  {
    if (child->get_address())
    {
      ossia::value name = ossia::net::osc_address_string(*child).substr(pos);
      ossia::value val = child->get_address()->fetch_value();

      std::vector<t_atom> va;
      value2atom vm{va};

      name.apply(vm);
      val.apply(vm);

      outlet_anything(x->x_dumpout, prependsym, va.size(), va.data());
    }
  }
}

/**
 * @brief obj_set send value to given address
 * @details For view and model, used to send value to a given address
 * @param x
 * @param s
 * @param argc
 * @param argv
 */
void obj_set(t_obj_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (argc > 0 && argv[0].a_type == A_SYMBOL)
  {
    std::string addr = argv[0].a_w.w_symbol->s_name;
    argv++;
    argc--;
    if (x->x_node)
    {
      auto tmp = x->x_node;
      auto nodes = ossia::net::find_nodes(*x->x_node, addr);
      for (auto n : nodes)
      {
        if (n->get_address()){
          t_matcher matcher{n,x};
          x->x_matchers.push_back(std::move(matcher));
        }
      }
      t_obj_base::obj_push(x,gensym(""),argc, argv);
      x->x_matchers.clear();
      x->x_node = tmp;
    }
  }
}

/**
 * @brief find_and_display_friend : find the object that defined the node and display it
 * @param x : object that hold the node we are looking for
 * @param patcher : starting point to seach a friend
 * @return true if we found a friend to display
 */
bool find_and_display_friend(t_obj_base* x)
{
  int found = 0;
  if (x->x_otype == Type::remote)
  {
    for (auto& rm_matcher : x->x_matchers)
    {
      for (auto param : ossia_pd::instance().params.copy())
      {
        for (auto& pa_matcher : param->x_matchers)
        {
          if ( rm_matcher == pa_matcher )
          {
            found++;

            if (found > 10)
            {
              pd_error(x, "We only display the first 10 connected nodes, and yes this is arbitrary for the sake of your eyes.");
            } else {
              // display it
              t_obj_base* obj = pa_matcher.get_parent();
              t_canvas* patcher = obj->x_obj.o_canvas;

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
  else if (x->x_otype == Type::view)
  {
    for (auto& vw_matcher : x->x_matchers)
    {
      for (auto model : ossia_pd::instance().models.copy())
      {
        for (auto& md_matcher : model->x_matchers)
        {
          if ( vw_matcher == md_matcher )
          {
            found++;

            if (found > 10)
            {
              pd_error(x, "We only display the first 10 connected nodes, and yes this is arbitrary for the sake of your eyes.");
            } else {
              // display it
              t_obj_base* obj = md_matcher.get_parent();
              t_canvas* patcher = obj->x_obj.o_canvas;

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

}
} // namespace
