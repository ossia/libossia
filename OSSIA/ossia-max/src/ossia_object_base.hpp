#pragma once
#include "ossia-max.hpp"

namespace ossia {
namespace max {
    
    struct t_object_base {
        
        t_object    m_object;
        void**      m_inlets{};
        void**      m_outlets{};
        
        ossia::net::node_base* m_node{};
        
        t_symbol*   m_name{};
        bool        m_absolute = false;
        bool        m_dead = false;     // wether this object is being deleted or not;
        
        t_clock*    m_clock{};
        t_clock*    m_regclock{};       // registration clock
        t_clock*    m_unregclock{};     // unregistration clock
    
        void setValue(const ossia::value& val);
        
        static void push(t_object_base* x, t_symbol*, int argc, t_atom* argv);
        static void bang(t_object_base* x);
        
        void isDeleted(const ossia::net::node_base& n);
        bool isRenamed();
        void renaming();
        void derenaming();
        
        static std::vector<t_object_base*>& quarantine()
        {
            static std::vector<t_object_base*> quarantine;
            return quarantine;
        }
        
        static std::vector<t_object_base*>& rename()
        {
            static std::vector<t_object_base*> rename;
            return rename;
        }
    };

} // max namespace
} // ossia namespace
