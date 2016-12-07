#pragma once

#include <ossia/ossia.hpp>
#include <sstream>

extern "C" {
#include <cicm_wrapper.h>
}

extern "C" void setup_ossia0x2emodel(void);
extern "C" void setup_ossia0x2edevice(void);
extern "C" void setup_ossia0x2eparam(void);
extern "C" void setup_ossia0x2eremote(void);

// define some useful OSSIA symbols
static t_symbol* osym_empty               = gensym("");
static t_symbol* osym_model               = gensym("ossia.model");
static t_symbol* osym_view                = gensym("ossia.view");
static t_symbol* osym_remote              = gensym("ossia.remote");
static t_symbol* osym_param               = gensym("ossia.param");
static t_symbol* osym_device              = gensym("ossia.device");

static t_symbol* osym_send_param          = gensym("__OSSIA__ossia.param");
static t_symbol* osym_send_remote         = gensym("__OSSIA__ossia.remote");
static t_symbol* osym_send_model          = gensym("__OSSIA__ossia.model");

using namespace ossia;

template <typename T>
struct value_visitor
{
    T* x;

    void operator()(Impulse) const
    {
        post("%s receive an impulse",x->x_name->s_name);
    }
    void operator()(int32_t i) const
    {
        post("%s receive an Integer %d",x->x_name->s_name, i);
    }
    void operator()(float f) const
    {
        t_atom a;
        SETFLOAT(&a,f);
        outlet_float(x->x_dataout, (t_float) f);
        outlet_anything(x->x_setout,gensym("set"),1,&a);
    }
    void operator()(bool b) const
    {
        post("%s receive a Boolean %d",x->x_name->s_name, b);
    }
    void operator()(const std::string& str) const
    {
        post("%s receive a String %s",x->x_name->s_name, str.c_str());
        t_symbol* s=gensym(str.c_str());
        t_atom a;
        SETSYMBOL(&a,s);
        outlet_symbol(x->x_dataout, s);
        outlet_anything(x->x_setout,gensym("set"),1,&a);
    }
    void operator()(char c) const
    {
        post("%s receive a Char %s",x->x_name->s_name, c);
        outlet_float(x->x_dataout, (float)c);
    }
    void operator()(Vec2f vec) const
    {
        post("%s receive a Vec2f (%.2f,%.2f)",x->x_name->s_name, vec[0], vec[1]);
    }
    void operator()(Vec3f vec) const
    {
        post("%s receive a Vec3f (%.2f,%.2f,%.2f)",x->x_name->s_name, vec[0], vec[1],vec[2]);
    }
    void operator()(Vec4f vec) const
    {
        post("%s receive a Vec4f (%.2f,%.2f,%.2f,%.2f)",x->x_name->s_name, vec[0], vec[1],vec[2],vec[3]);
    }
    void operator()(const Destination& d) const
    {
      post("%s receive a Destination",x->x_name->s_name);
      /*
      s << "destination" << ossia::net::address_string_from_node(d.value);
      if(d.unit)
      {
        s << " " << ossia::get_pretty_unit_text(d.unit);
      }
      */
    }
    void operator()(const std::vector<ossia::value>& t) const
    {
      //getTupleAsString(t, s);
      post("%s receive a Tuple",x->x_name->s_name);
    }
    void operator()() const
    {
        pd_error(x, "%s receive an invalid data",x->x_name->s_name);
    }
};

/**
 * @brief The obj_hierachy class
 * @details Little class to store object pointer and hierarchy level, useful for iterating object from top to bottom.
 */
class obj_hierachy{
public:
    t_pd* x;
    int hierarchy;
    friend bool operator<(obj_hierachy a, obj_hierachy b){
        return a.hierarchy < b.hierarchy;
    }
};

/*!
 * \fn                static t_class* find_parent(t_eobj* x, t_symbol* classname)
 * \brief             Find the first instance of classname beside or above (in a parent patcher) context.
 * \details           The function iterate all objects at the same level or above x and return the first instance of classname found.
 * \param x           The object around which to search.
 * \param classname   The name of the object object we are looking for.
 * \param start_level Level above current object where to start. 0 for current patcher, 1 start searching in parent canvas.
 * \return The instance of the found object.
 */
static t_pd* find_parent(t_eobj* x, t_symbol* classname, int start_level=0){
    t_canvas* canvas = x->o_canvas;

    while(canvas && start_level--){
        canvas = canvas->gl_owner;
    }

    while (canvas){
        t_gobj* list = canvas->gl_list;
        while(list){
            if (list->g_pd->c_name == classname){
                return &(list->g_pd);
            }
            list = list->g_next;
        }
        canvas = canvas->gl_owner;
    }
    return nullptr;
}

template<typename T>
static T* find_parent_alive(t_eobj* x, t_symbol* classname, int start_level = 0){
    T* obj = (T*) find_parent(x,classname, start_level);
    if (obj){
        while (obj && obj->x_dead ){
            obj = find_parent_alive<T>(&obj->x_obj,classname, 1);
        }
    }
    return obj;
}

/**
 * @brief Find all objects [classname] in the current patcher starting at specified level.
 * @param list : list in which we are looking for objecfts
 * @param classname : name of the object to search
 * @param starting_level : 0 to start searching in current canvas, -1 to start searching from the parent canvas
 * @return std::vector<t_pd*> containing pointer to t_pd struct of the corresponding classname
 */
static std::vector<obj_hierachy> find_child(t_gobj* list, t_symbol* classname, int starting_level = 0){
    std::vector<obj_hierachy> found;
    while (list && list->g_pd){
        if ( list->g_pd->c_name == gensym("canvas")){
            t_canvas* canvas = (t_canvas*) &list->g_pd;
            if(!canvas_istable(canvas)){
                t_gobj* _list = canvas->gl_list;
                std::vector<obj_hierachy> found_tmp = find_child(_list, classname, 0);
                for (auto obj : found_tmp){
                    obj.hierarchy++; // increase hierarchy of objects found in a subpatcher
                    found.push_back(obj);
                }
            }
        } else if ( list->g_pd->c_name == classname ){
            obj_hierachy oh;
            oh.hierarchy = 0;
            oh.x = &list->g_pd;
            found.push_back(oh);
        }
        list=list->g_next;
    }

    return found;
}

/**
 * @brief return relative path to corresponding object
 * @param x
 * @param classname
 * @param found_obj
 * @param address
 * @return
 */
static bool get_relative_path(t_eobj* x, t_symbol* classname, t_class** found_obj, std::string& address){
    std::vector<std::string> ss;
    std::stringstream hierarchy;

    bool result=false;
    t_canvas* canvas = x->o_canvas;
    do{
        t_gobj* list = canvas->gl_list;
        ss.push_back(canvas->gl_name->s_name);
        while(list){
            if (list->g_pd->c_name == classname){
                *found_obj = list->g_pd;
                result = true;
                break;
            }
            list = list->g_next;
        }
        canvas = canvas->gl_owner;
    } while (canvas);

    auto rit = ss.rbegin();
    for ( ; rit != ss.rend() ; ++rit){
        hierarchy << *rit << "/";
    }
    address = hierarchy.str();
    return result;
}

/**
 * @brief get_absolute_path
 * @param node
 * @return std::string with full path to node from root device in an OSC style (with '/')
 */
static std::string get_absolute_path(ossia::net::node_base* node)
{
    std::vector<std::string> vs;
    while (node){
        std::string name = node->getName(); // FIXME why this crash ?
        vs.push_back(name);
        node = node->getParent();
    }
    std::stringstream fullpath;
    fullpath << "/";
    auto rit = vs.rbegin();
    for ( ; rit != vs.rend() ; ++rit){
        fullpath << *rit << "/";
    }
    return fullpath.str();
}
