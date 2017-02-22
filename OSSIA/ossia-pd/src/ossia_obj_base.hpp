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
    bool        x_dead = false; // wether this object is being deleted or not;


    ossia::net::node_base* x_node{};
    void setValue(const ossia::value& val);

    void isDeleted(const ossia::net::node_base& n);

    void quarantining();
    void dequarantining();
    bool isQuarantined();

    // FIXME t_remote and t_param are together in the same basket
    // this lead to thusday crash
    static std::vector<ossia_obj_base*>& quarantine(){
        static std::vector<ossia_obj_base*> quarantine;
        return quarantine;
    }
};

} } // namespace
