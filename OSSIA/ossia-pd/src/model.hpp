#pragma once

#include "ossia_obj_base.hpp"
#include "ossia-pd.hpp"
#include "device.hpp"

namespace ossia { namespace pd {

struct t_model : ossia_obj_base
{
    bool register_node(ossia::net::node_base*  node);
    bool do_registration(ossia::net::node_base*  node);
    bool unregister();

    static std::vector<t_model*>& quarantine(){
        static std::vector<t_model*> quarantine;
        return quarantine;
    }
    t_symbol* x_tags;
    t_symbol* x_description;

    void isDeleted(const ossia::net::node_base& n)
    {
        if (!x_dead){
            x_node = nullptr;
            obj_quarantining<t_model>(this);
            obj_register<t_model>(this);
        }
    }
};

} } // namespace
