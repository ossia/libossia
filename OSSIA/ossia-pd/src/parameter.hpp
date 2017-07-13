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
    bool x_hidden;

    static ossia::safe_vector<t_param*>& quarantine(){
        static ossia::safe_vector<t_param*> quarantine;
        return quarantine;
    }

    void isDeleted(const ossia::net::node_base& n)
    {
        x_node->about_to_be_deleted.disconnect<t_param, &t_param::isDeleted>(this);
        x_node = nullptr;
        obj_quarantining<t_param>(this);
    }

    static ossia::safe_vector<t_param*>& rename(){
        static ossia::safe_vector<t_param*> rename;
        return rename;
    }

    bool isRenamed(t_param* x){
      return x->rename().contains(x);
    }

    void renaming(t_param* x){
        if ( !isRenamed(x) ) x->rename().push_back(x);
    }

    void derenaming(t_param* x){
        x->rename().remove_all(x);
    }
};

} } // namespace
