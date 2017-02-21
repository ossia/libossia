#pragma once

#include "ossia-pd.hpp"
#include "ossia_obj_base.hpp"
#include "device.hpp"

namespace ossia { namespace pd {

struct t_view : ossia_obj_base
{
    bool register_node(ossia::net::node_base*  node);
    bool unregister();

    void _register();

    void isDeleted(const ossia::net::node_base& n){
        x_node = nullptr;
        quarantining();
    }
};

static void view_loadbang(t_view *x){
    x->_register();
}

} } // namespace



