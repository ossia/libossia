#pragma once

#include "ossia_obj_base.hpp"
#include "ossia-pd.hpp"
#include "device.hpp"

namespace ossia { namespace pd {

struct t_model : ossia_obj_base
{
    bool register_node(ossia::net::node_base*  node);
    bool unregister();
};

} } // namespace
