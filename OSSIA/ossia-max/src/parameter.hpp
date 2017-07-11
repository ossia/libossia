#pragma once

#include "ossia_object_base.hpp"

namespace ossia {
    
    namespace max {
        
# pragma mark -
# pragma mark t_parameter structure declaration
        
        struct t_parameter : t_object_base
        {
            t_symbol*   m_type;
            long        m_type_size;
            t_atom      m_default[64];
            
            // TODO use optional for range
            t_atom      m_range[2];
            t_symbol*   m_bounding_mode;
            t_symbol*   m_access_mode;
            bool        m_repetition_filter;
            t_symbol*   m_unit;
            t_symbol*   m_tags;
            t_symbol*   m_description;
            long        m_priority;
            
            bool register_node(ossia::net::node_base* node);
            
            bool do_registration(ossia::net::node_base* node);
            
            bool unregister();
            
            void is_deleted(const ossia::net::node_base& n);
            
            bool is_renamed(t_parameter*);
            
            void renaming(t_parameter* x);
            
            void derenaming(t_parameter* x);
            
            static std::vector<t_parameter*>& quarantine()
            {
                static std::vector<t_parameter*> quarantine;
                return quarantine;
            }
            
            static std::vector<t_parameter*>& rename()
            {
                static std::vector<t_parameter*> rename;
                return rename;
            }
        };
             
# pragma mark -
# pragma mark Utilities
        
        static void push_default_value(t_parameter* x)
        {
            int i = 0;
            
            for (; i<x->m_type_size ; i++)
            {
                if (x->m_default[i].a_type == A_NOTHING) break;
            }
            
            if (i > 0)
                t_object_base::push(x, nullptr, i, x->m_default);
        }
        
} // max namespace
} // ossia namespace

# pragma mark -
# pragma mark ossia_parameter class declaration

extern "C"
{
    void* ossia_parameter_new(t_symbol*, long, t_atom*);
    void ossia_parameter_free(ossia::max::t_parameter*);

    void ossia_parameter_assist(ossia::max::t_parameter*, void*, long, long, char*);
    void ossia_parameter_in_float(ossia::max::t_parameter*, double f);
    void ossia_parameter_in_int(ossia::max::t_parameter*, long int f);
    void ossia_parameter_in_bang(ossia::max::t_parameter*);
    void ossia_parameter_in_symbol(ossia::max::t_parameter*, t_symbol*);
    void ossia_parameter_in_char(ossia::max::t_parameter*, char);
    void ossia_parameter_in_anything(ossia::max::t_parameter*, t_symbol*, long, t_atom*);
}
