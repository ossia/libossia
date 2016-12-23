#include "ossia_obj_base.hpp"

namespace ossia { namespace pd {

void ossia_obj_base::setValue(const ossia::value& v){
    value_visitor<ossia_obj_base> vm;
    vm.x = (ossia_obj_base*) &x_obj;
    v.apply(vm);
}

void ossia_obj_base :: isDeleted(const ossia::net::node_base& n)
{
    x_node = nullptr;
}

} } // namespace
