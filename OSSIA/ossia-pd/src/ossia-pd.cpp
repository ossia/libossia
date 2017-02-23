/*
// Copyright (c) 2016 Antoine Villeret
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

#include <m_pd.h> // puredata header
#include "ossia-pd.hpp"
#include "model.hpp"
#include "device.hpp"
#include "view.hpp"
#include "remote.hpp"
#include "parameter.hpp"

static t_class *ossia_class;

namespace ossia { namespace pd {

typedef struct t_ossia
{
    t_object    m_obj; // pd object - always placed in first in the object's struct

} t_ossia;

static void *ossia_new(t_symbol *name, int argc, t_atom *argv)
{
    t_ossia *x = (t_ossia *)pd_new(ossia_class);
    return (x);
}

extern "C" void ossia_setup(void)
{
    t_class* c = class_new(gensym("ossia"),
                           (t_newmethod)ossia_new, nullptr,
                           sizeof(t_ossia), CLASS_DEFAULT, A_GIMME, 0);
    post("Welcome to ossia library");
    ossia_class = c;

    setup_ossia0x2emodel();
    setup_ossia0x2edevice();
    setup_ossia0x2eparam();
    setup_ossia0x2eremote();
    setup_ossia0x2eview();
}

template<typename T> void obj_dump_path(T *x)
{
    t_atom a;
    std::string fullpath = get_absolute_path(x->x_node);
    SETSYMBOL(&a,gensym(fullpath.c_str()));
    outlet_anything(x->x_dumpout,gensym("fullpath"), 1, &a);
}
template void obj_dump_path<t_param> (t_param  *x);
template void obj_dump_path<t_remote> (t_remote  *x);
template void obj_dump_path<t_model> (t_model  *x);
template void obj_dump_path<t_view> (t_view  *x);


// self registering (when creating the object)
template<typename T> bool obj_register(T *x)
{
    if (x->x_node) return true; // already registered

    int l;
    t_device *device = (t_device*) find_parent(&x->x_obj,osym_device, 0, &l);

    // first try to locate a ossia.device in the parent hierarchy...
    if (!device) {
        return false; // not ready to register : if there is no device, nothing could be registered
    }

    t_model *model = nullptr;
    t_view *view = nullptr;
    int view_level=0, model_level=0;

    // then try to locate a view or a parameter when x is ossia.view or ossia.remote
    model = find_parent_alive<t_model>(&x->x_obj,osym_model, 0, &model_level);
    if (std::is_same<T,t_view>::value || std::is_same<T,t_remote>::value) {
        // param = (t_param*) find_parent(&x->x_obj, osym_param, 0, &param_level);
        // TODO : search param and test level agains view/model level
        view = find_parent_alive<t_view>(&x->x_obj,osym_view, 0, &view_level);
    }

    ossia::net::node_base*  node = nullptr;

    if (view){
        node = view->x_node;
    } else if (model){
        node = model->x_node;
    } else {
        node = device->x_node;
    }

    return x->register_node(node);
}

template bool obj_register<t_param> (t_param *x);
template bool obj_register<t_remote>(t_remote *x);
template bool obj_register<t_model> (t_model *x);
template bool obj_register<t_view>  (t_view *x);

template<typename T> void obj_dump(T *x){
    t_atom a;
    std::string fullpath = get_absolute_path(x->x_node);
    SETSYMBOL(&a,gensym(fullpath.c_str()));
    outlet_anything(x->x_dumpout,gensym("fullpath"), 1, &a);

    if ( x->x_node ){
        SETFLOAT(&a, 1.);
    } else {
        SETFLOAT(&a, 0.);
    }
    outlet_anything(x->x_dumpout,gensym("registered"), 1, &a);

    SETFLOAT(&a, obj_isQuarantined<T>(x));
    outlet_anything(x->x_dumpout,gensym("quarantined"), 1, &a);
}

template void obj_dump<t_param> (t_param  *x);
template void obj_dump<t_model> (t_model  *x);
template void obj_dump<t_remote>(t_remote *x);
template void obj_dump<t_view>  (t_view   *x);

template<typename T> void obj_quarantining(T* x){
    if ( !obj_isQuarantined<T>(x) ) x->quarantine().push_back(x);
}
template void obj_quarantining<t_param> (t_param  *x);
template void obj_quarantining<t_model> (t_model  *x);
template void obj_quarantining<t_remote>(t_remote *x);
template void obj_quarantining<t_view>  (t_view   *x);

template<typename T> void obj_dequarantining(T* x){
    x->quarantine().erase(std::remove(x->quarantine().begin(), x->quarantine().end(), x), x->quarantine().end());
}
template void obj_dequarantining<t_param> (t_param  *x);
template void obj_dequarantining<t_model> (t_model  *x);
template void obj_dequarantining<t_remote>(t_remote *x);
template void obj_dequarantining<t_view>  (t_view   *x);

template<typename T> bool obj_isQuarantined(T* x){
    return std::find(x->quarantine().begin(), x->quarantine().end(), x) != x->quarantine().end();
}
template bool obj_isQuarantined<t_param> (t_param  *x);
template bool obj_isQuarantined<t_model> (t_model  *x);
template bool obj_isQuarantined<t_remote>(t_remote *x);
template bool obj_isQuarantined<t_view>  (t_view   *x);

} } // namespace
