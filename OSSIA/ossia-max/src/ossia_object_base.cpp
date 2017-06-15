#include "ossia_object_base.hpp"

namespace ossia {
namespace max {
    
    void t_object_base :: set_value(const ossia::value& v)
    {
        value_visitor<t_object_base> vm;
        vm.x = (t_object_base*) &m_object;
        v.apply(vm);
    }
    
    void t_object_base :: push(t_object_base *x, t_symbol* , int argc, t_atom* argv)
    {
        if (x->m_node && x->m_node->get_address())
        {
            if (argc==1)
            {
                // convert one element array to single element
                if (argv->a_type == A_SYM)
                    x->m_node->get_address()->push_value(std::string(atom_getsym(argv)->s_name));
                
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
            x->set_value(x->m_node->get_address()->value());
    }
    
    /*
    std::vector<object_hierachy> find_child_to_register(t_object_base* x, t_gobj* start_list, std::string classname)
    {
        std::string subclassname = classname == "ossia.model" ? "ossia.parameter" : "ossia.remote";
        
        t_gobj* list = start_list;
        std::vector<object_hierachy> found;
        
        // 1: iterate object list and look for ossia.model / ossia.view object
        while (list && list->g_pd)
        {
            std::string current = list->g_pd->c_name->s_name;
            if (current == classname)
            {
                object_hierachy oh;
                oh.hierarchy = 0;
                oh.x = (t_obj_base*) &list->g_pd;
                oh.classname = classname;
                // TODO check if object is dying
                if ( x != oh.x && !oh.x->x_dead)
                {
                    t_object_base* o = oh.x;
                    found.push_back(oh);
                }
            }
            list=list->g_next;
        }
        
        // 2: if there is no ossia.model / ossia.view in the current patch, look into the subpatches
        
        if (found.empty())
        {
            list = start_list;
            while (list && list->g_pd)
            {
                std::string current = list->g_pd->c_name->s_name;
                if (current == "canvas")
                {
                    t_canvas* canvas = (t_canvas*) &list->g_pd;
                    if(!canvas_istable(canvas))
                    {
                        t_gobj* _list = canvas->gl_list;
                        std::vector<object_hierachy> found_tmp = find_child_to_register(x, _list, classname);
                        for (auto obj : found_tmp)
                        {
                            obj.hierarchy++; // increase hierarchy of objects found in a subpatcher
                            found.push_back(obj);
                        }
                    }
                }
                list=list->g_next;
            }
            
            // 3: finally look for ossia.param / ossia.remote in the same pather
            list = start_list;
            while (list && list->g_pd)
            {
                std::string current = list->g_pd->c_name->s_name;
                
                if (current == subclassname)
                {
                    object_hierachy oh;
                    oh.hierarchy = 0;
                    oh.x = (t_obj_base*) &list->g_pd;
                    oh.classname = subclassname;
                    if (x != oh.x)
                    {
                        found.push_back(oh);
                    }
                }
                
                list=list->g_next;
            }
        }
        
        return found;
    }
    */
} // max namespace
} // ossia namespace
