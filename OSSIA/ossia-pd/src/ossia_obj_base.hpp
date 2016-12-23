#pragma once
#include "ossia-pd.hpp"

namespace ossia { namespace pd {

struct ossia_obj_base {
    t_eobj      x_obj;
    t_symbol*   x_name{};
    t_outlet*   x_setout{};
    t_outlet*   x_dataout{};
    t_outlet*   x_dumpout{};
    bool        x_absolute = false;

    ossia::net::node_base* x_node{};
    void setValue(const ossia::value& val);
};

} } // namespace
