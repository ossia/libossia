#pragma once
#include "ossia-pd.hpp"

namespace ossia { namespace pd {

struct t_obj_base {
    t_eobj      x_obj;
    t_symbol*   x_name{};
    t_outlet*   x_setout{};
    t_outlet*   x_dataout{};
    t_outlet*   x_dumpout{};
    bool        x_absolute = false;
    bool        x_dead = false; // wether this object is being deleted or not;

    t_clock*  x_clock{};
    t_canvas* x_last_opened_canvas{};
    std::chrono::milliseconds x_last_click;

    ossia::net::node_base* x_node{};
    void setValue(const ossia::value& val);
    static void obj_push(t_obj_base* x, t_symbol*, int argc, t_atom* argv);
    static void obj_bang(t_obj_base* x);
};

bool find_and_display_friend(t_obj_base* x, t_canvas* patcher);
void obj_tick(t_obj_base* x);

} } // namespace
