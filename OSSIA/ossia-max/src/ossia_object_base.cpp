#include "ossia_object_base.hpp"

namespace ossia {
namespace max {
    
    void t_object_base :: apply_value_visitor(const ossia::value& v)
    {
        value_visitor<t_object_base> vm;
        vm.x = this;
        v.apply(vm);
    }
    
    void t_object_base :: push(t_object_base *x, t_symbol* , int argc, t_atom* argv)
    {
        if (x->m_node && x->m_node->get_address())
        {
            if (argc == 1)
            {
                // convert one element array to single element
                if (argv->a_type == A_SYM)
                    x->m_node->get_address()->push_value(std::string(atom_getsym(argv)->s_name));
                
                else if (argv->a_type == A_LONG)
                    x->m_node->get_address()->push_value((int32_t)atom_getlong(argv));
                
                else if (argv->a_type == A_FLOAT)
                    x->m_node->get_address()->push_value(atom_getfloat(argv));
            }
            else
            {
                std::vector<ossia::value> list;
                
                for (; argc > 0 ; argc--, argv++)
                {
                    if (argv->a_type == A_SYM)
                        list.push_back(std::string(atom_getsym(argv)->s_name));
                    else if (argv->a_type == A_LONG)
                        list.push_back((int32_t)atom_getlong(argv));
                    else if (argv->a_type == A_FLOAT)
                        list.push_back(atom_getfloat(argv));
                    else object_error((t_object*)x,"value type not handled");
                }
                
                x->m_node->get_address()->push_value(list);
            }
        }
    }
    
    void t_object_base :: bang(t_object_base *x)
    {
        if (x->m_node && x->m_node->get_address())
            x->apply_value_visitor(x->m_node->get_address()->value());
    }
/*
    void t_object_base :: tick(t_object_base* x)
    {
        if (x->m_last_opened_canvas)
        {
            glist_noselect(x->m_last_opened_canvas);
            x->m_last_opened_canvas = nullptr;
        }
    }
*/
} // max namespace
} // ossia namespace
