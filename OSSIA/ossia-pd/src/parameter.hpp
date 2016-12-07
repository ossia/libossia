#pragma once

#include "ossia-pd.hpp"
#include "device.hpp"
#include "model.hpp"

using namespace ossia;

struct t_param
{
    t_eobj      x_obj;

    t_symbol* x_type;
    t_atom x_default;
    // TODO use optional for range
    // TODO add clipmode (or bounding ?) attribute
    float range[2];

    t_symbol*   x_name;
    t_outlet*   x_setout;
    t_outlet*   x_dataout;
    t_outlet*   x_dumpout;
    bool        x_absolute;

    ossia::net::node_base* x_node;
    ossia::net::address_base* x_localAddress;
    void setValue(const ossia::value& val);
    bool register_node(ossia::net::node_base* node);
    bool unregister();
};

// self registering (at loadbang or when creating the object)
template<typename T>
static bool obj_register(T *x)
{
    if (x->x_node) return true; // already registered

    // first try to locate a ossia.model in the parent hierarchy...
    t_model *model;
    t_device *device = (t_device*) find_parent(&x->x_obj,osym_device, 0);
    ossia::net::node_base*  node = nullptr;

    if (!device) {
        std::cerr << "no device at all, abording" << std::endl;
        return false; // not ready to register : if there is no device, model will be unable to register too
    }

    if ((model = find_parent_alive<t_model>(&x->x_obj,osym_model, 0))){
        node = model->x_node;
    } else {
        node = device->x_node;
    }

    return x->register_node(node);
}
