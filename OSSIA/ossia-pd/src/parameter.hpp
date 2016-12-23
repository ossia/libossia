#pragma once

#include "ossia_obj_base.hpp"
#include "device.hpp"
#include "model.hpp"

namespace ossia { namespace pd {

struct t_param : ossia_obj_base
{
    bool register_node(ossia::net::node_base* node);
    bool unregister();

    t_symbol* x_type{};
    t_atom x_default;
    // TODO use optional for range
    // TODO add clipmode (or bounding ?) attribute
    float range[2];
};

} } // namespace
