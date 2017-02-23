#include "ossia_obj_base.hpp"

namespace ossia { namespace pd {

void obj_base::setValue(const ossia::value& v){
    value_visitor<obj_base> vm;
    vm.x = (obj_base*) &x_obj;
    v.apply(vm);
}

void obj_base::obj_push(obj_base *x, t_symbol* , int argc, t_atom* argv){
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

void obj_base::obj_bang(obj_base *x){
  if ( x->x_node && x->x_node->getAddress() ) x->setValue(x->x_node->getAddress()->cloneValue());
}

} } // namespace
