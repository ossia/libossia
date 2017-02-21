#pragma once
#include "ossia_obj_base.hpp"
#include "device.hpp"
#include <boost/optional.hpp>

namespace ossia { namespace pd {

struct t_remote : ossia_obj_base
{
    bool register_node(ossia::net::node_base* node);
    bool unregister();

    boost::optional<ossia::callback_container<ossia::value_callback>::iterator> x_callbackit;

    void isDeleted(const ossia::net::node_base& n){
        unregister();
    }

};

static void remote_loadbang(t_remote *x){
    bool res = obj_register<t_remote>(x);
}

} } // namespace
