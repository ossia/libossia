#include "ossia_obj_base.hpp"

extern void glist_noselect(t_glist *x);
extern void canvas_vis(t_canvas *x, t_floatarg f);
extern void canvas_editmode(t_canvas *x, t_floatarg state);
extern void glist_select(t_glist *x, t_gobj *y);

namespace ossia { namespace pd {

void t_obj_base::setValue(const ossia::value& v){
    value_visitor<t_obj_base> vm;
    vm.x = (t_obj_base*) &x_obj;
    v.apply(vm);
}

void t_obj_base::obj_push(t_obj_base *x, t_symbol* , int argc, t_atom* argv){
  if ( x->x_node && x->x_node->getAddress() ){
      if (argc==1){
          // convert one element array to single element
          if (argv->a_type == A_SYMBOL)
              x->x_node->getAddress()->pushValue(std::string(atom_getsymbol(argv)->s_name));
          else if (argv->a_type == A_FLOAT)
              x->x_node->getAddress()->pushValue(atom_getfloat(argv));
      } else {
          std::vector<ossia::value> list;
          for (; argc > 0 ; argc--, argv++){
              if (argv->a_type == A_SYMBOL)
                  list.push_back(std::string(atom_getsymbol(argv)->s_name));
              else if (argv->a_type == A_FLOAT)
                  list.push_back(atom_getfloat(argv));
              else pd_error(x,"value type not handled");
          }
          x->x_node->getAddress()->pushValue(list);
      }
  }
}

void obj_tick(t_obj_base* x){
  if (x->x_last_opened_canvas){
    glist_noselect(x->x_last_opened_canvas);
    x->x_last_opened_canvas = nullptr;
  }
}

void t_obj_base::obj_bang(t_obj_base *x){
  if ( x->x_node && x->x_node->getAddress() ) x->setValue(x->x_node->getAddress()->cloneValue());
}

bool find_and_display_friend(t_obj_base* x, t_canvas* patcher){
  t_gobj* list = patcher->gl_list;

  std::string target_str;
  std::string canvas_str = "canvas";
  std::string xclassname = std::string(x->x_obj.o_obj.te_g.g_pd->c_name->s_name);
  if ( xclassname == "ossia.remote" ) target_str = "ossia.param";
  else if (xclassname == "ossia.view" ) target_str = "ossia.model";

  while (list && list->g_pd){
    std::string classname = list->g_pd->c_name->s_name;
    if ( classname == target_str ) {
      t_obj_base* p = (t_obj_base*) list;
      if (p->x_node == x->x_node) {
        if (x->x_last_opened_canvas) glist_noselect(x->x_last_opened_canvas);
        if (x->x_clock) clock_unset(x->x_clock);
        glist_noselect(patcher);
        x->x_last_opened_canvas = patcher;
        canvas_vis(glist_getcanvas(patcher), 1);
        glist_select(patcher, &p->x_obj.o_obj.te_g);
        if (x->x_clock) clock_delay(x->x_clock, 1000);
        return true;
      }
    } else if ( classname == canvas_str ){
      t_canvas* canvas = (t_canvas*) &list->g_pd;
      if(!canvas_istable(canvas)){
        t_gobj* _list = canvas->gl_list;
        if(find_and_display_friend(x, canvas)) return true;
      }
    }
    list=list->g_next;
  }
  return false;
}
} } // namespace
