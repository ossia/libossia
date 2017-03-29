/*
// Copyright (c) 2016 Antoine Villeret
// For information on usage and redistribution, and for a DISCLAIMER OF ALL
// WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*/

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

void register_quarantinized(){
    for (auto model : t_model::quarantine()){
        obj_register<t_model>(static_cast<t_model*>(model));
    }
    for (auto param : t_param::quarantine()){
        obj_register<t_param>(static_cast<t_param*>(param));
    }
    for (auto view : t_view::quarantine()){
        obj_register<t_view>(static_cast<t_view*>(view));
    }
    for (auto remote : t_remote::quarantine()){
        obj_register<t_remote>(static_cast<t_remote*>(remote));
    }
}

std::vector<obj_hierachy> find_child_to_register(t_obj_base* x, t_gobj* start_list, std::string classname){
    std::string subclassname = classname == "ossia.model" ? "ossia.param" : "ossia.remote";

    t_gobj* list = start_list;
    std::vector<obj_hierachy> found;

    // 1: iterate object list and look for ossia.model / ossia.view object
    while (list && list->g_pd){
        std::string current = list->g_pd->c_name->s_name;
        if ( current == classname ) {
            obj_hierachy oh;
            oh.hierarchy = 0;
            oh.x = (t_obj_base*) &list->g_pd;
            oh.classname = classname;
            // TODO check if object is dying
            if ( x != oh.x && !oh.x->x_dead){
                t_obj_base* o = oh.x;
                found.push_back(oh);
            }
        }
        list=list->g_next;
    }

    // 2: if there is no ossia.model / ossia.view in the current patch, look into the subpatches

    if(found.empty()){
        list = start_list;
        while (list && list->g_pd){
            std::string current = list->g_pd->c_name->s_name;
            if ( current == "canvas" ){
                t_canvas* canvas = (t_canvas*) &list->g_pd;
                if(!canvas_istable(canvas)){
                    t_gobj* _list = canvas->gl_list;
                    std::vector<obj_hierachy> found_tmp = find_child_to_register(x, _list, classname);
                    for (auto obj : found_tmp){
                        obj.hierarchy++; // increase hierarchy of objects found in a subpatcher
                        found.push_back(obj);
                    }
                }
            }
            list=list->g_next;
        }

        // 3: finally look for ossia.param / ossia.remote in the same pather
        list = start_list;
        while (list && list->g_pd){
            std::string current = list->g_pd->c_name->s_name;
            if ( current == subclassname ) {
                obj_hierachy oh;
                oh.hierarchy = 0;
                oh.x = (t_obj_base*) &list->g_pd;
                oh.classname = subclassname;
                if ( x != oh.x ) {
                    found.push_back(oh);
                }
            }
            list=list->g_next;
        }
    }

    return found;
}

template<typename T> std::string get_absolute_path(T* x)
{
    std::vector<std::string> vs;

    t_model *model = nullptr;
    t_view *view = nullptr;
    int view_level=0, model_level=0;
    std::stringstream fullpath;

    if (std::is_same<T,t_view>::value || std::is_same<T,t_remote>::value) {
        int start_level = 0;
        if (std::is_same<T,t_view>::value) start_level = 1;
        view = find_parent_alive<t_view>(&x->x_obj,"ossia.view", start_level, &view_level);
        t_view* tmp = nullptr;
        while (view){
            vs.push_back(view->x_name->s_name);
            tmp = view;
            view = find_parent_alive<t_view>(&tmp->x_obj,"ossia.view", 1, &view_level);
        }
        t_eobj* obj = nullptr;
        if(tmp) obj = &tmp->x_obj;
        else obj = &x->x_obj;
        int l = 0;
        t_device *device = (t_device*) find_parent(obj,"ossia.device", 0, &l);
        if(device) fullpath << device->x_name->s_name << ":";
    } else {
        int start_level = 0;
        if (std::is_same<T,t_model>::value) start_level = 1;
        model = find_parent_alive<t_model>(&x->x_obj,"ossia.model", start_level, &model_level);
        t_model * tmp = nullptr;
        while (model){
            vs.push_back(model->x_name->s_name);
            tmp = model;
            model = find_parent_alive<t_model>(&tmp->x_obj,"ossia.model", 1, &model_level);
        }
        t_eobj* obj = nullptr;
        if(tmp) obj = &tmp->x_obj;
        else obj = &x->x_obj;
        int l = 0;
        t_device *device = nullptr;
        if(obj) device = (t_device*) find_parent(obj,"ossia.device", 0, &l);
        if(device) fullpath << device->x_name->s_name << ":";
    }

    auto rit = vs.rbegin();
    for ( ; rit != vs.rend() ; ++rit){
        fullpath  << "/" << *rit;
    }
    if (vs.empty()) fullpath  << "/";
    return fullpath.str();
}
template std::string get_absolute_path<t_param> (t_param *x);
template std::string get_absolute_path<t_remote>(t_remote *x);
template std::string get_absolute_path<t_model> (t_model *x);
template std::string get_absolute_path<t_view>  (t_view *x);

// self registering (when creating the object)
template<typename T> bool obj_register(T *x)
{
    if (x->x_node) return true; // already registered
    if (x->x_dead) return false; // object will be removed soon

    int l;
    t_device *device = (t_device*) find_parent(&x->x_obj,"ossia.device", 0, &l);

    // first try to locate a ossia.device in the parent hierarchy...
    if (!device) {
        return false; // not ready to register : if there is no device, nothing could be registered
    }

    t_model *model = nullptr;
    t_view *view = nullptr;
    int view_level=0, model_level=0;

    if (!x->x_absolute){
        // then try to locate a parent view or model
        if (std::is_same<T,t_view>::value || std::is_same<T,t_remote>::value) {
            view = find_parent_alive<t_view>(&x->x_obj,"ossia.view", 0, &view_level);
        } else {
            model = find_parent_alive<t_model>(&x->x_obj,"ossia.model", 0, &model_level);
        }
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
    std::string fullpath ;
    if (x->x_node){
        fullpath = ossia::net::address_string_from_node(*x->x_node);
        SETSYMBOL(&a,gensym(fullpath.c_str()));
        outlet_anything(x->x_dumpout,gensym("fullpath"), 1, &a);
    }
    fullpath = get_absolute_path<T>(x);
    fullpath += "/";
    fullpath += x->x_name->s_name;
    SETSYMBOL(&a,gensym(fullpath.c_str()));
    outlet_anything(x->x_dumpout,gensym("absolutepath"), 1, &a);

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
  return ossia::contains(x->quarantine(),x);
}
template bool obj_isQuarantined<t_param> (t_param  *x);
template bool obj_isQuarantined<t_model> (t_model  *x);
template bool obj_isQuarantined<t_remote>(t_remote *x);
template bool obj_isQuarantined<t_view>  (t_view   *x);

} } // namespace
