#include "ossia_obj_base.hpp"

namespace ossia { namespace pd {

void obj_base::setValue(const ossia::value& v){
    value_visitor<obj_base> vm;
    vm.x = (obj_base*) &x_obj;
    v.apply(vm);
}

} } // namespace
