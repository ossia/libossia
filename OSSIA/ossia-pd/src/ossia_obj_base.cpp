// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia_obj_base.hpp"

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

void obj_tick(t_obj_base* x)
{
  if (x->x_last_opened_canvas)
  {
    glist_noselect(x->x_last_opened_canvas);
    x->x_last_opened_canvas = nullptr;
  }
}

void t_obj_base::obj_bang(t_obj_base* x)
{
  if (x->x_node && x->x_node->get_address())
    x->setValue(x->x_node->get_address()->value());
}

}
} // namespace
