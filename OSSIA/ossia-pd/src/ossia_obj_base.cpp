#include "ossia_obj_base.hpp"

void ossia_obj_base::setValue(const ossia::value& v){
    value_visitor<ossia_obj_base> vm;
    vm.x = (ossia_obj_base*) &x_obj;
    v.apply(vm);
}
