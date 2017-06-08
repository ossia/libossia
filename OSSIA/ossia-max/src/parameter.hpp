#pragma once

#include "ossia_object_base.hpp"
//#include "device.hpp"
//#include "model.hpp"

namespace ossia {
    
    namespace max {
        
# pragma mark -
# pragma mark t_parameter structure declaration
        
        struct t_parameter : t_object_base
        {
            t_symbol*   m_type;
            int         m_type_size;
            t_atom      m_default[64];
            
            // TODO use optional for range
            float       m_range[2];
            t_symbol*   m_bounding_mode;
            t_symbol*   m_access_mode;
            bool        m_repetition_filter;
            t_symbol*   m_unit;
            t_symbol*   m_tags;
            t_symbol*   m_description;
            int         m_priority;
            
            bool register_node(ossia::net::node_base* node);
            bool do_registration(ossia::net::node_base* node);
            bool unregister();
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

#define data_out 0

extern "C"
{
    void* ossia_parameter_new(t_symbol *s, long argc, t_atom *argv);
    void ossia_parameter_free(ossia::max::t_parameter* x);
    
    void ossia_parameter_assist(ossia::max::t_parameter *x, void *b, long m, long a, char *s);
    void ossia_parameter_in_float(ossia::max::t_parameter* x, double f);
    void ossia_parameter_in_int(ossia::max::t_parameter* x, long int f);
    void ossia_parameter_in_bang(ossia::max::t_parameter* x);
    void ossia_parameter_in_symbol(ossia::max::t_parameter* x, t_symbol* f);
    void ossia_parameter_in_char(ossia::max::t_parameter* x, char f);
    void ossia_parameter_in_anything(ossia::max::t_parameter* x, t_symbol *s, long argc, t_atom *argv);
}
