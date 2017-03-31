#pragma once
#include <ossia/ossia.hpp>
#include <sstream>

extern "C" {
#include <cicm_wrapper.h>
}

namespace ossia { namespace pd {

extern "C" void setup_ossia0x2eclient(void);
extern "C" void setup_ossia0x2edevice(void);
extern "C" void setup_ossia0x2emodel(void);
extern "C" void setup_ossia0x2eparam(void);
extern "C" void setup_ossia0x2eremote(void);
extern "C" void setup_ossia0x2eview(void);

struct t_obj_base;

struct value2atom
{
    std::vector<t_atom> operator()(impulse) const
    {
        t_atom a;
        SETSYMBOL(&a, gensym("bang"));
        std::vector<t_atom> va;
        va.push_back(a);
        return va;
    }
    std::vector<t_atom> operator()(int32_t i) const
    {
        t_atom a;
        SETFLOAT(&a, (t_float) i);
        std::vector<t_atom> va;
        va.push_back(a);
        return va;
    }
    std::vector<t_atom> operator()(float f) const
    {
        t_atom a;
        SETFLOAT(&a,f);
        std::vector<t_atom> va;
        va.push_back(a);
        return va;
    }
    std::vector<t_atom> operator()(bool b) const
    {
        t_atom a;
        t_float f = b?1.:0.;
        SETFLOAT(&a, f);
        std::vector<t_atom> va;
        va.push_back(a);
        return va;
    }
    std::vector<t_atom> operator()(const std::string& str) const
    {
        t_symbol* s=gensym(str.c_str());
        t_atom a;
        SETSYMBOL(&a,s);
        std::vector<t_atom> va;
        va.push_back(a);
        return va;
    }
    std::vector<t_atom> operator()(char c) const
    {
        std::vector<t_atom> va;
        t_atom a;
        SETFLOAT(&a, (float)c);
        va.push_back(a);
        return va;
    }
    std::vector<t_atom> operator()(vec2f vec) const
    {
        t_atom a[2];
        SETFLOAT(a,vec[0]);
        SETFLOAT(a+1,vec[1]);
        std::vector<t_atom> va;
        va.push_back(a[0]);
        va.push_back(a[1]);
        return va;
    }
    std::vector<t_atom> operator()(vec3f vec) const
    {
        t_atom a[3];
        SETFLOAT(a,vec[0]);
        SETFLOAT(a+1,vec[1]);
        SETFLOAT(a+2,vec[2]);
        std::vector<t_atom> va;
        va.push_back(a[0]);
        va.push_back(a[1]);
        va.push_back(a[2]);
        return va;
    }
    std::vector<t_atom> operator()(vec4f vec) const
    {
        t_atom a[4];
        SETFLOAT(a,vec[0]);
        SETFLOAT(a+1,vec[1]);
        SETFLOAT(a+2,vec[2]);
        SETFLOAT(a+3,vec[3]);
        std::vector<t_atom> va;
        va.push_back(a[0]);
        va.push_back(a[1]);
        va.push_back(a[2]);
        return va;
    }
    std::vector<t_atom> operator()(const Destination& d) const
    {
      /*
      s << "destination" << ossia::net::address_string_from_node(d.value);
      if(d.unit)
      {
        s << " " << ossia::get_pretty_unit_text(d.unit);
      }
      */
      std::vector<t_atom> va;
      return va;
    }
    std::vector<t_atom> operator()(const std::vector<ossia::value>& t) const
    {
        std::vector<t_atom> va;
        for (auto v : t){
            std::vector<t_atom> b;
            value2atom vm;
            b = v.apply(vm);
            std::move(b.begin(), b.end(), std::back_inserter(va));
        }
        return va;

    }
    std::vector<t_atom> operator()() const
    {
        std::vector<t_atom> va;
        return va;
    }
};

template <typename T>
struct value_visitor
{
    T* x;

    void operator()(impulse) const
    {
        // TODO how to deal with impulse ? in Pd bang object doesn't have [set ...( message
        // and sending a bang to the bang object connected to the inlet of the sender will lead to stack overflow...
        outlet_bang(x->x_dataout);
        if(x->x_setout) outlet_bang(x->x_setout);
    }
    void operator()(int32_t i) const
    {
        t_atom a;
        SETFLOAT(&a, (t_float) i);
        outlet_float(x->x_dataout, (t_float) i);
        if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),1,&a);
    }
    void operator()(float f) const
    {
        t_atom a;
        SETFLOAT(&a,f);
        outlet_float(x->x_dataout, (t_float) f);
        if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),1,&a);
    }
    void operator()(bool b) const
    {
        t_atom a;
        t_float f = b?1.:0.;
        SETFLOAT(&a, f);
        outlet_float(x->x_dataout, f);
        if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),1,&a);
    }
    void operator()(const std::string& str) const
    {
        t_symbol* s=gensym(str.c_str());
        t_atom a;
        SETSYMBOL(&a,s);
        outlet_symbol(x->x_dataout, s);
        if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),1,&a);
    }
    void operator()(char c) const
    {
        t_atom a;
        SETFLOAT(&a, (float)c);
        outlet_float(x->x_dataout, (float)c);
        if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),1,&a);
    }
    void operator()(vec2f vec) const
    {
        t_atom a[2];
        SETFLOAT(a,vec[0]);
        SETFLOAT(a+1,vec[1]);
        outlet_list(x->x_dataout, gensym("list"), 2, a);
        if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),2, a);
    }
    void operator()(vec3f vec) const
    {
        t_atom a[3];
        SETFLOAT(a,vec[0]);
        SETFLOAT(a+1,vec[1]);
        SETFLOAT(a+2,vec[2]);
        outlet_list(x->x_dataout, gensym("list"), 3, a);
        if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),3, a);
    }
    void operator()(vec4f vec) const
    {
        t_atom a[4];
        SETFLOAT(a,vec[0]);
        SETFLOAT(a+1,vec[1]);
        SETFLOAT(a+2,vec[2]);
        SETFLOAT(a+3,vec[3]);
        outlet_list(x->x_dataout, gensym("list"), 4, a);
        if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),4, a);
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
      std::vector<t_atom> va;
      for (auto v : t){
          std::vector<t_atom> b;
          value2atom vm;
          b = v.apply(vm);
          std::move(b.begin(), b.end(), std::back_inserter(va));
      }
      outlet_list(x->x_dataout, gensym("list"), va.size(), &va[0]);
      if(x->x_setout) outlet_anything(x->x_setout,gensym("set"),va.size(), &va[0]);

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
    t_obj_base* x;
    int hierarchy;
    std::string classname;
    friend bool operator<(obj_hierachy a, obj_hierachy b){
        return a.hierarchy < b.hierarchy;
    }
};

/**
 * @brief register_quarantinized Try to register all quarantinized objects
 */
void register_quarantinized();

/**
 * @fn                static t_class* find_parent(t_eobj* x, t_symbol* classname)
 * @brief             Find the first instance of classname beside or above (in a parent patcher) context.
 * @details           The function iterate all objects at the same level or above x and return the first instance of classname found.
 * @param x           The object around which to search.
 * @param classname   The name of the object object we are looking for.
 * @param start_level Level above current object where to start. 0 for current patcher, 1 start searching in parent canvas.
 * @param level       Return level of the found object
 * @return The instance of the found object.
 */
static t_pd* find_parent(t_eobj* x, std::string classname, int start_level, int* level){
    t_canvas* canvas = x->o_canvas;

    *level = start_level;

    while(canvas && start_level){
        canvas = canvas->gl_owner; // gl_owner seems to be corrupted on the root canvas : canvas has no value
        start_level--;
    }

    if (start_level > 0) return nullptr; // if we can't reach start level (because we reach the root canvas before the start_level) then abort

    while (canvas != 0){
        t_gobj* list = canvas->gl_list;
        while(list){
            std::string current = list->g_pd->c_name->s_name;
            if (list->g_pd && (current==classname) &&  (&(list->g_pd) != &(x->o_obj.te_g.g_pd))){
                return &(list->g_pd);
            }
            list = list->g_next;
        }
        canvas = canvas->gl_owner;
        *level++;
    }
    return nullptr;
}

/**
 * @brief find_parent_alive
 * @details Find a parent that is not being removed soon
 * @param x
 * @param classname
 * @param start_level
 * @return
 */
template<typename T>
static T* find_parent_alive(t_eobj* x, std::string classname, int start_level, int* level){
    T* obj = (T*) find_parent(x,classname, start_level, level);
    if (obj){
        while (obj && obj->x_dead ){
            obj = find_parent_alive<T>(&obj->x_obj,classname, 1, level);
        }
    }
    return obj;
}

/**
 * @brief Find all objects [classname] in the current patcher starting at specified level.
 * @param list : list in which we are looking for objecfts
 * @param classname : name of the object to search (ossia.model or ossia.view)
 * @return std::vector<t_pd*> containing pointer to t_pd struct of the corresponding classname
 */
std::vector<obj_hierachy> find_child_to_register(t_obj_base* x, t_gobj* start_list, std::string classname);

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
 * @param t_obj_base
 * @return std::string with full path to object from root device in an OSC style (with '/')
 */
template<typename T> std::string get_absolute_path(T* x);

// we can't have virtual methods with C linkage so we need a bunch a template instead...
template<typename T> extern bool obj_register(T *x);
template<typename T> extern void obj_bang(T *x);
template<typename T> extern void obj_dump(T *x);

template<typename T> extern void obj_quarantining(T* x);
template<typename T> extern void obj_dequarantining(T* x);
template<typename T> extern bool obj_isQuarantined(T* x);

static std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol){
    std::vector<std::string> tags;

    if (tags_symbol){
        char* c = tags_symbol->s_name;
        std::string tag="";

        while (*c!='\0'){
            if (*c==' '){
                tags.push_back(tag);
                tag = std::string("");
            } else tag += *c;
            c++;
        }
    }
    return tags;
}

} } // namespace
