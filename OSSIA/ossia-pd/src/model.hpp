#pragma once

#include "ossia-pd.hpp"
#include "device.hpp"

struct t_model
{
    t_eobj      x_obj;
    t_symbol*   x_name;
    bool        x_absolute;
    bool        x_dead; // wether this object is dying or not;
    t_outlet*   dumpout;

    ossia::net::node_base* x_node;
    bool register_node(ossia::net::node_base*  node);
    bool unregister();
};
