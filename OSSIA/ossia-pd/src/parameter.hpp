#pragma once

#include "ossia_obj_base.hpp"
#include "device.hpp"
#include "model.hpp"

namespace ossia { namespace pd {

struct t_param : t_obj_base
{
    bool register_node(ossia::net::node_base* node);
    bool do_registration(ossia::net::node_base* node);
    bool unregister();

    t_symbol* x_type;
    int x_type_size;
    t_atom x_default[64];
    // TODO use optional for range
    float x_range[2];
    t_symbol* x_bounding_mode;
    t_symbol* x_access_mode;
    t_float x_repetition_filter=0;
    t_symbol* x_unit;
    t_symbol* x_tags;
    t_symbol* x_description;
    int x_priority;

    static std::vector<t_param*>& quarantine(){
        static std::vector<t_param*> quarantine;
        return quarantine;
    }

    void isDeleted(const ossia::net::node_base& n)
    {
        x_node->aboutToBeDeleted.disconnect<t_param, &t_param::isDeleted>(this);
        x_node = nullptr;
        obj_quarantining<t_param>(this);
    }
};

} } // namespace
