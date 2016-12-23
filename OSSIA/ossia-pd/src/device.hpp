#pragma once

#include "ossia-pd.hpp"
#include "ossia_obj_base.hpp"

namespace ossia { namespace pd {

struct t_device : ossia_obj_base
{
    t_symbol* x_remoteip{};
    t_symbol* x_protocol{};
    int x_remoteport;
    int x_localport;
    ossia::net::generic_device* x_device{};
    ossia::net::local_protocol x_local_proto;

    void register_children();
    void unregister_children();

    void addressCreationHandler(const ossia::net::address_base& n);
    void nodeCreationHandler(const ossia::net::node_base& n);
};

} } // namespace
