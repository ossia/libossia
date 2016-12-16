#pragma once

#include "ossia-pd.hpp"
#include "device.hpp"
#include "model.hpp"

using namespace ossia;

struct t_param
{
    t_eobj      x_obj;

    t_symbol* x_type{};
    t_atom x_default;
    // TODO use optional for range
    // TODO add clipmode (or bounding ?) attribute
    float range[2];

    t_symbol*   x_name{};
    t_outlet*   x_setout{};
    t_outlet*   x_dataout{};
    t_outlet*   x_dumpout{};
    bool        x_absolute{};

    ossia::net::node_base* x_node{};
    ossia::net::address_base* x_localAddress{};
    void setValue(const ossia::value& val);
    bool register_node(ossia::net::node_base* node);
    bool unregister();
    void isDeleted(const ossia::net::node_base& n);
};
