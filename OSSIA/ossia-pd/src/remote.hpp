#pragma once
#include "ossia-pd.hpp"
#include "device.hpp"
#include "boost/optional.hpp"

using namespace ossia;

struct t_remote
{
    t_eobj      x_obj;

    t_symbol*   x_name;
    t_outlet*   x_setout;
    t_outlet*   x_dataout;
    t_outlet*   x_dumpout;
    bool        x_absolute;

    ossia::net::node_base* x_node;
    void setValue(const ossia::value& val);
    bool register_node(ossia::net::node_base* node);
    bool unregister();
    boost::optional<ossia::callback_container<ossia::value_callback>::iterator> x_callbackit;
};
