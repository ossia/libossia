#pragma once

#include "ossia-pd.hpp"
#include "ossia_obj_base.hpp"
#include "device.hpp"

namespace ossia { namespace pd {

struct t_view : t_obj_base
{
    bool register_node(ossia::net::node_base*  node);
    bool do_registration(ossia::net::node_base*  node);
    bool unregister();

    void isDeleted(const ossia::net::node_base& n){
        if (!x_dead){
            x_node = nullptr;
            obj_quarantining<t_view>(this);
            obj_register<t_view>(this);
        }
    }

    static std::vector<t_view*>& quarantine(){
        static std::vector<t_view*> quarantine;
        return quarantine;
    }
};

} } // namespace



