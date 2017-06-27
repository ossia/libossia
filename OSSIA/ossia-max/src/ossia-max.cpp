#include <ossia-max/src/ossia-max.hpp>
#include <ossia-max/src/client.hpp>
#include <ossia-max/src/device.hpp>
#include <ossia-max/src/logger.hpp>
#include <ossia-max/src/model.hpp>
#include <ossia-max/src/parameter.hpp>

# pragma mark -
# pragma mark library

using namespace ossia::max;

// ossia-max library loading
extern "C"
void ext_main(void *r)
{
    ossia_client_setup();
    ossia_device_setup();
    ossia_logger_setup();
    ossia_model_setup();
    ossia_parameter_setup();
    
    post("OSSIA library for Max is loaded");
}

// ossia-max library constructor
ossia_max::ossia_max()
{}

// ossia-max library instance
ossia_max& ossia_max::instance()
{
    static ossia_max library_instance;
    return library_instance;
}

namespace ossia {
namespace max {
    
# pragma mark -
# pragma mark Templates

    template<typename T>
    bool object_register(T* x)
    {
        post("OSSIA library : object_register TODO");
        
        if (x->m_node) return true; // already registered
        if (x->m_dead) return false; // object will be removed soon
        
        int l;
        t_device* device = (t_device*) find_parent((t_object*)x, gensym("ossia.device"), 0, &l);
        t_client* client = (t_client*) find_parent((t_object*)x, gensym("ossia.client"), 0, &l);
        /*
        // first try to locate a ossia.device in the parent hierarchy...
        if (!device && !client)
            return false; // not ready to register : if there is no device, nothing could be registered
        
        t_model* model = nullptr;
        t_view* view = nullptr;
        int view_level = 0, model_level = 0;
        
        if (!x->m_absolute)
        {
            // then try to locate a parent view or model
            if (std::is_same<T, t_view>::value || std::is_same<T, t_remote>::value)
                view = find_parent_alive<t_view>(&x->m_object, gensym("ossia.view"), 0, &view_level);
            else
                model = find_parent_alive<t_model>(&x->m_object, gensym("ossia.model"), 0, &model_level);
        }
        
        ossia::net::node_base*  node = nullptr;
        
        if (view)
            node = view->m_node;
        else if (model)
            node = model->m_node;
        else if (client)
            node = client->m_node;
        else
            node = device->m_node;
        
        return x->register_node(node);
    */
    
        return false;
    }
    
    template bool object_register<t_parameter> (t_parameter*);
    template bool object_register<t_model> (t_model*);
    
    template<typename T>
    std::string object_path(T* x)
    {
        post("OSSIA library : object_register TODO");
        /*
    std::vector<std::string> vs;
    
    t_model* model = nullptr;
    t_view* view = nullptr;
    int view_level = 0, model_level = 0;
    std::stringstream fullpath;
    
    if (std::is_same<T, t_view>::value || std::is_same<T, t_remote>::value)
    {
        int start_level = 0;
        if (std::is_same<T, t_view>::value)
            start_level = 1;
        
        view = find_parent_alive<t_view>(&x->m_object, gensym("ossia.view"), start_level, &view_level);
        t_view* tmp = nullptr;
        
        while (view)
        {
            vs.push_back(view->m_name->s_name);
            tmp = view;
            view = find_parent_alive<t_view>(&tmp->m_object, gensym("ossia.view"), 1, &view_level);
        }
        
        t_object* object = nullptr;
        
        if (tmp)
            object = &tmp->m_object;
        else
            object = &m_object;
        
        int l = 0;
        t_device* device = (t_device*) find_parent(object, gensym("ossia.device"), 0, &l);
        t_client* client = (t_client*) find_parent(object, gensym("ossia.client"), 0, &l);
        
        if (client)
            fullpath << client->m_name->s_name << ":";
        else if (device)
            fullpath << device->m_name->s_name << ":";
    }
    else
    {
        int start_level = 0;
        if (std::is_same<T, t_model>::value)
            start_level = 1;
        
        model = find_parent_alive<t_model>(&x->m_object, gensym("ossia.model"), start_level, &model_level);
        t_model* tmp = nullptr;
        
        while (model)
        {
            vs.push_back(model->m_name->s_name);
            tmp = model;
            model = find_parent_alive<t_model>(&tmp->m_object, gensym("ossia.model"), 1, &model_level);
        }
        
        t_object* object = nullptr;
        if(tmp)
            object = &tmp->m_object;
        else
            object = &m_object;
        
        int l = 0;
        t_device *device = nullptr;
        
        if (object)
            device = (t_device*) find_parent(object, gensym("ossia.device"), 0, &l);
        
        if (device)
            fullpath << device->m_name->s_name << ":";
    }
    
    auto rit = vs.rbegin();
    for ( ; rit != vs.rend() ; ++rit)
    {
        fullpath  << "/" << *rit;
    }
    
    if (vs.empty())
        fullpath  << "/";
    
    return fullpath.str();
     */
    
        std::stringstream fullpath;
        return fullpath.str();
    }
    
    template std::string object_path<t_parameter> (t_parameter*);
    template std::string object_path<t_model> (t_model*);
    
    template<typename T>
    void object_quarantining(T* x)
    {
        if ( !object_is_quarantined<T>(x) ) x->quarantine().push_back(x);
    }
    
    template void object_quarantining<t_parameter> (t_parameter*);
    template void object_quarantining<t_model> (t_model*);
    
    template<typename T>
    void object_dequarantining(T* x)
    {
        x->quarantine().erase(std::remove(x->quarantine().begin(), x->quarantine().end(), x), x->quarantine().end());
    }
    
    template void object_dequarantining<t_parameter> (t_parameter*);
     template void object_dequarantining<t_model> (t_model*);
    
    template<typename T>
    bool object_is_quarantined(T* x)
    {
        return ossia::contains(x->quarantine(), x);
    }
    
    template bool object_is_quarantined<t_parameter> (t_parameter*);
    template bool object_is_quarantined<t_model> (t_model*);
    
    template<typename T>
    void object_dump(T* x)
    {
        t_atom a;
        std::string fullpath ;
        
        if (x->m_node)
        {
            fullpath = ossia::net::address_string_from_node(*x->m_node);
            atom_setsym(&a, gensym(fullpath.c_str()));
            outlet_anything(x->m_dump_out, gensym("fullpath"), 1, &a);
        }
        
        fullpath = object_path(x);
        if (fullpath.back() != '/')
            fullpath += "/";
        
        fullpath += x->m_name->s_name;
        atom_setsym(&a, gensym(fullpath.c_str()));
        outlet_anything(x->m_dump_out, gensym("maxpath"), 1, &a);
        
        if (x->m_node)
            atom_setfloat(&a, 1.);
        else
            atom_setfloat(&a, 0.);
        
        outlet_anything(x->m_dump_out, gensym("registered"), 1, &a);
        
        atom_setfloat(&a, object_is_quarantined(x));
        outlet_anything(x->m_dump_out, gensym("quarantined"), 1, &a);
        
        if (x->m_node)
        {
            ossia::net::address_base* address = x->m_node->get_address();
            if (address)
            {
                // type
                std::string type = "unknown";
                switch (address->get_value_type())
                {
                    case ossia::val_type::FLOAT:
                        type = "float";
                        break;
                    case ossia::val_type::INT:
                        type = " int";
                        break;
                    case ossia::val_type::VEC2F:
                        type = "vec2f";
                        break;
                    case ossia::val_type::VEC3F:
                        type = "vec3f";
                        break;
                    case ossia::val_type::VEC4F:
                        type = "vec4f";
                        break;
                    case ossia::val_type::IMPULSE:
                        type = "impulse";
                        break;
                    case ossia::val_type::BOOL:
                        type = "bool";
                        break;
                    case ossia::val_type::STRING:
                        type = "string";
                        break;
                    case ossia::val_type::TUPLE:
                        type = "tuple";
                        break;
                    case ossia::val_type::CHAR:
                        type = "char";
                        break;
                    default:
                        type = "unknown";
                }
                
                atom_setsym(&a, gensym(type.c_str()));
                outlet_anything(x->m_dump_out, gensym("type"), 1, &a);
                
                // domain
                ossia::domain domain = address->get_domain();
                atom_setsym(&a, gensym(domain.to_pretty_string().c_str()));
                outlet_anything(x->m_dump_out, gensym("domain"), 1, &a);
                
                // bounding mode
                std::string bounding_mode;
                switch(address->get_bounding())
                {
                    case ossia::bounding_mode::FREE:
                        bounding_mode = "free";
                        break;
                    case ossia::bounding_mode::CLIP:
                        bounding_mode = "clip";
                        break;
                    case ossia::bounding_mode::WRAP:
                        bounding_mode = "wrap";
                        break;
                    case ossia::bounding_mode::FOLD:
                        bounding_mode = "fold";
                        break;
                    case ossia::bounding_mode::LOW:
                        bounding_mode = "low";
                        break;
                    case ossia::bounding_mode::HIGH:
                        bounding_mode = "high";
                        break;
                    default:
                        bounding_mode = "unknown";
                }
                
                atom_setsym(&a, gensym(bounding_mode.c_str()));
                outlet_anything(x->m_dump_out, gensym("bounding_mode"), 1, &a);
                
                // access mode
                std::string access_mode;
                switch(address->get_access())
                {
                    case ossia::access_mode::BI:
                        access_mode = "bi";
                        break;
                    case ossia::access_mode::GET:
                        access_mode = "get";
                        break;
                    case ossia::access_mode::SET:
                        access_mode = "set";
                        break;
                    default:
                        access_mode = "unknown";
                }
                
                atom_setsym(&a, gensym(access_mode.c_str()));
                outlet_anything(x->m_dump_out, gensym("access_mode"), 1, &a);
                
                // repetition filter
                bool rep = address->get_repetition_filter();
                atom_setfloat(&a, rep);
                outlet_anything(x->m_dump_out,gensym("repetition_filter"), 1, &a);
                
                // unit
                // TODO
                address->get_unit();
            }
            
            // description
            auto description = ossia::net::get_description(*(x->m_node));
            if (description)
            {
                atom_setsym(&a, gensym((*description).c_str()));
                outlet_anything(x->m_dump_out, gensym("description"), 1, &a);
            }
            else
                outlet_anything(x->m_dump_out, gensym("tags"), 0, nullptr);
            
            // tags
            auto tags = ossia::net::get_tags(*x->m_node);
            if (tags)
            {
                std::size_t N = (*tags).size();
                std::vector<t_atom> l(N);
                for(std::size_t i = 0; i < N; i++)
                {
                    atom_setsym(&l[i], gensym((*tags)[i].c_str()));
                }
                outlet_anything(x->m_dump_out, gensym("tags"), N, l.data());
            }
            else
                outlet_anything(x->m_dump_out, gensym("tags"), 0, nullptr);
        }
    }
    
    template void object_dump<t_parameter> (t_parameter*);
    template void object_dump<t_model> (t_model*);
    
# pragma mark -
# pragma mark Utilities
    
    
    void register_quarantinized()
    {
        for (auto model : t_model::quarantine())
        {
            object_register<t_model>(static_cast<t_model*>(model));
        }
        
        for (auto parameter : t_parameter::quarantine())
        {
            object_register<t_parameter>(static_cast<t_parameter*>(parameter));
        }
        /*
        for (auto view : t_view::quarantine())
        {
            object_register<t_view>(static_cast<t_view*>(view));
        }
        
        for (auto remote : t_remote::quarantine())
        {
            object_register<t_remote>(static_cast<t_remote*>(remote));
        }
         */
    }
    
    t_object* find_parent(t_object* x, t_symbol* classname, int start_level, int* level)
    {
        t_object* parent = nullptr;
        
        // look upper if there is an upper level and if it is not the level where to start the research
        t_object* patcher = get_patcher(x);
        
        if (patcher && start_level)
            parent = find_parent(patcher, classname, start_level--, level);
        
        // if no parent object have been found in upper patcher, look around
        if (!parent)
        {
            t_object* obj = object_attr_getobj(patcher, _sym_firstobject);
            
            while (obj)
            {
                if (object_attr_getsym(obj, _sym_maxclass) == classname)
                {
                    parent = obj;
                    *level = start_level;
                    break;
                }
                
                obj = object_attr_getobj(obj, _sym_nextobject);
            }
        }

        return parent;
    }
    
    t_object* get_patcher(t_object *obj)
    {
        t_object *patcher = NULL;
        object_obex_lookup(obj, gensym("#P"), &patcher);
        
        // If obj is in a bpatcher, the patcher is NULL
        if (!patcher)
        {
            patcher = object_attr_getobj(obj, _sym_parentpatcher);
        }
        
        return patcher;
    }
    
    std::vector<std::string> parse_tags_symbol(t_symbol* tags_symbol)
    {
        std::vector<std::string> tags;
        
        if (tags_symbol)
        {
            char* c = tags_symbol->s_name;
            std::string tag="";
            
            while (*c!='\0')
            {
                if (*c==' ')
                {
                    tags.push_back(tag);
                    tag = std::string("");
                }
                else tag += *c;
                
                c++;
            }
            tags.push_back(tag);
        }
        
        return tags;
    }
    
} // max namespace
} // ossia namespace
