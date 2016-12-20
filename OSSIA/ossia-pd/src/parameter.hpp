#pragma once

#include "ossia_obj_base.hpp"
#include "device.hpp"
#include "model.hpp"

using namespace ossia;

struct t_param : ossia_obj_base
{
    bool register_node(ossia::net::node_base* node);
    bool unregister();
    void isDeleted(const ossia::net::node_base& n);

    ossia::net::address_base* x_localAddress{};
    t_symbol* x_type{};
    t_atom x_default;
    // TODO use optional for range
    // TODO add clipmode (or bounding ?) attribute
    float range[2];
};
