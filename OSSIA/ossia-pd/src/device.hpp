#pragma once

#include "ossia-pd.hpp"

struct t_device
{
    t_eobj      x_obj;
    t_symbol*   x_name;
    t_symbol* x_remoteip;
    t_outlet* x_dumpout;
    t_symbol* x_protocol;
    int x_remoteport;
    int x_localport;
    ossia::net::generic_device *x_device;
    ossia::net::node_base*  x_node;
    ossia::net::local_protocol x_local_proto;

    void register_children();
    void unregister_children();
};
