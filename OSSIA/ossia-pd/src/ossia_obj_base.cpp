// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_obj_base.hpp"
#include "utils.hpp"

extern void glist_noselect(t_glist* x);
extern void canvas_vis(t_canvas* x, t_floatarg f);
extern void canvas_editmode(t_canvas* x, t_floatarg state);
extern void glist_select(t_glist* x, t_gobj* y);

namespace ossia
{
namespace pd
{

void t_obj_base::setValue(const ossia::value& v)
{
  value_visitor<t_obj_base> vm;
  vm.x = (t_obj_base*)&x_obj;
  v.apply(vm);
}

/**
 * @brief t_obj_base::obj_push : push a value to a node
 * @param x : caller that holds the node to push to
 * @param argc : number of value in the list
 * @param argv :  list of t_atom value(s)
 */
void t_obj_base::obj_push(t_obj_base* x, t_symbol*, int argc, t_atom* argv)
{
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

/**
 * @brief obj_tick deselect last selected object
 * @details used by ø.remote and ø.view when displaying connected parent
 * @param x
 */
void obj_tick(t_obj_base* x)
{
  if (x->x_last_opened_canvas)
  {
    glist_noselect(x->x_last_opened_canvas);
    x->x_last_opened_canvas = nullptr;
  }
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
 * @brief list_all_child : list all node childs addresses recursively
 * @param node : starting point
 * @param list : reference to a string vector to store each address
 */
void list_all_child(const ossia::net::node_base& node, std::vector<std::string>& list){
  for (const auto& child : node.children_copy())
  {
    if (auto addr = child->get_address())
    {
      std::string s = ossia::net::osc_address_string(*child);
      list.push_back(s);
    }
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
  std::vector<std::string> list;
  list_all_child(*x->x_node, list);
  int pos = ossia::net::osc_address_string(*x->x_node).length();
  for (auto& addr : list)
  {
    std::string s = addr.substr(pos);
    t_atom a;
    SETSYMBOL(&a,gensym(s.c_str()));
    outlet_anything(x->x_dumpout, prependsym,1,&a);
  }
}

/**
 * @brief find_and_display_friend : find the object that defined the node and display it
 * @param x : object that hold the node we are looking for
 * @param patcher : starting point to seach a friend
 * @return true if we found a friend to display
 */
// TODO refactor this to use ossia_pd::instance().params|remotes
// instead of going through all objects in all patchers.
bool find_and_display_friend(t_obj_base* x, t_canvas* patcher)
{
  t_gobj* list = patcher->gl_list;

  std::string target_str;
  std::string canvas_str = "canvas";
  std::string xclassname
      = std::string(x->x_obj.o_obj.te_g.g_pd->c_name->s_name);
  if (xclassname == "ossia.remote")
    target_str = "ossia.param";
  else if (xclassname == "ossia.view")
    target_str = "ossia.model";

  while (list && list->g_pd)
  {
    std::string classname = list->g_pd->c_name->s_name;
    if (classname == target_str)
    {
      t_obj_base* p = (t_obj_base*)list;
      if (p->x_node == x->x_node)
      {
        if (x->x_last_opened_canvas)
          glist_noselect(x->x_last_opened_canvas);
        if (x->x_clock)
          clock_unset(x->x_clock);
        glist_noselect(patcher);
        x->x_last_opened_canvas = patcher;
        canvas_vis(glist_getcanvas(patcher), 1);
        glist_select(patcher, &p->x_obj.o_obj.te_g);
        if (x->x_clock)
          clock_delay(x->x_clock, 1000);
        return true;
      }
    }
    else if (classname == canvas_str)
    {
      t_canvas* canvas = (t_canvas*)&list->g_pd;
      if (!canvas_istable(canvas))
      {
        t_gobj* _list = canvas->gl_list;
        if (find_and_display_friend(x, canvas))
          return true;
      }
    }
    list = list->g_next;
  }
  return false;
}

}
} // namespace
