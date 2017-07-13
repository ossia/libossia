#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia/editor/dataspace/dataspace_visitors.hpp>

using namespace ossia::max;

# pragma mark -
# pragma mark ossia_parameter class methods

extern "C"
void ossia_parameter_setup()
{
    auto& ossia_library = ossia_max::instance();
    
    // instantiate the ossia.parameter class
    ossia_library.ossia_parameter_class = class_new("ossia.parameter",
                                                    (method)ossia_parameter_new,
                                                    (method)ossia_parameter_free,
                                                    (long)sizeof(ossia::max::t_parameter),
                                                    0L, A_GIMME, 0);
    
    class_addmethod(ossia_library.ossia_parameter_class, (method)ossia_parameter_assist,        "assist",       A_CANT, 0);
    class_addmethod(ossia_library.ossia_parameter_class, (method)ossia_parameter_in_bang,       "bang",         0);
    class_addmethod(ossia_library.ossia_parameter_class, (method)ossia_parameter_in_int,        "int",          A_LONG, 0);
    class_addmethod(ossia_library.ossia_parameter_class, (method)ossia_parameter_in_float,      "float",        A_FLOAT, 0);
    class_addmethod(ossia_library.ossia_parameter_class, (method)ossia_parameter_in_symbol,     "symbol",       A_SYM, 0);
    class_addmethod(ossia_library.ossia_parameter_class, (method)ossia_parameter_in_anything,   "anything",     A_GIMME, 0);
    
    CLASS_ATTR_SYM(ossia_library.ossia_parameter_class, "type", 0, t_parameter, m_type);
    CLASS_ATTR_LONG(ossia_library.ossia_parameter_class, "type_size", 0, t_parameter, m_type_size);
    CLASS_ATTR_ATOM_ARRAY(ossia_library.ossia_parameter_class, "default", 0, t_parameter, m_default, 64);
    CLASS_ATTR_ATOM_ARRAY(ossia_library.ossia_parameter_class, "range", 0, t_parameter, m_range, 2);
    CLASS_ATTR_SYM(ossia_library.ossia_parameter_class, "bounding_mode", 0, t_parameter, m_bounding_mode);
    CLASS_ATTR_SYM(ossia_library.ossia_parameter_class, "access_mode", 0, t_parameter, m_access_mode);
    CLASS_ATTR_LONG(ossia_library.ossia_parameter_class, "repetition_filter", 0, t_parameter, m_repetition_filter);
    CLASS_ATTR_SYM(ossia_library.ossia_parameter_class, "unit", 0, t_parameter, m_unit);
    CLASS_ATTR_SYM(ossia_library.ossia_parameter_class, "tags", 0, t_parameter, m_tags);
    CLASS_ATTR_SYM(ossia_library.ossia_parameter_class, "description", 0, t_parameter, m_description);
    CLASS_ATTR_LONG(ossia_library.ossia_parameter_class, "priority", 0, t_parameter, m_priority);
    
    // TODO : for each attribute : CLASS_ATTR_ADD_FLAGS(ossia_library.ossia_parameter_class, "attrname", ATTR_SET_OPAQUE);
    // note : ATTR_SET_OPAQUE means that the attribute can't be changed by message afterward
    
    CLASS_ATTR_STYLE(ossia_library.ossia_parameter_class, "repetition_filter", 0, "onoff");
    
    class_register(CLASS_BOX, ossia_library.ossia_parameter_class);
}

extern "C"
void* ossia_parameter_new(t_symbol *s, long argc, t_atom *argv)
{
    auto& ossia_library = ossia_max::instance();
    t_parameter* x = (t_parameter *) object_alloc(ossia_library.ossia_parameter_class);
    
    if (x)
    {
        // make outlets
        x->m_dump_out = outlet_new(x, NULL);						// anything outlet to dump parameter state
        x->m_set_out = outlet_new(x, NULL);                         // anything outlet to output data for ui
        x->m_data_out = outlet_new(x, NULL);						// anything outlet to output data
        
        x->m_node = nullptr;
        
        // initialize attributes
        atom_setfloat(&x->m_range[0], 0.);
        atom_setfloat(&x->m_range[1], 1.);
        x->m_access_mode = gensym("BI");
        x->m_bounding_mode = gensym("FREE");
        x->m_unit = gensym("");
        x->m_type = gensym("tuple");
        x->m_type_size = 1;
        x->m_tags = gensym("");
        x->m_description = gensym("");
        x->m_priority = 0;
        
        x->m_clock = clock_new(x, (method)ossia::max::push_default_value);
        
        // parse arguments
        long attrstart = attr_args_offset(argc, argv);
        
        // check name argument
        x->m_name = _sym_nothing;
        if (attrstart && argv)
        {
            if (atom_gettype(argv) == A_SYM)
            {
                x->m_name = atom_getsym(argv);
                x->m_absolute = std::string(x->m_name->s_name) != "" && x->m_name->s_name[0] == '/';
            }
        }
        
        if (x->m_name == _sym_nothing)
        {
            object_error((t_object*)x, "needs a name as first argument");
            x->m_name = gensym("untitledParameter");
            return x;
        }
        
        // process attr args, if any
        attr_args_process(x, argc-attrstart, argv+attrstart);
        
        // start registration
        object_register<t_parameter>(x);
    }
    
    return x;
}

extern "C"
void ossia_parameter_free(t_parameter* x)
{
    x->unregister();
    object_dequarantining(x);
    object_free(x->m_clock);
    // TODO : free outlets
}

extern "C"
void ossia_parameter_assist(t_parameter *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
    {
        sprintf(s, "I am inlet %ld", a);
    }
    else
    {
        sprintf(s, "I am outlet %ld", a);
    }
}

template<typename T>
void ossia_parameter_in(t_parameter* x, T f)
{
    if (x && x->m_node)
    {
        if (auto addr = x->m_node->get_address())
        {
            addr->push_value(f);
        }
    }
}

extern "C"
void ossia_parameter_in_float(t_parameter* x, double f)
{ ossia_parameter_in(x, f); }

extern "C"
void ossia_parameter_in_int(t_parameter* x, long int f)
{ ossia_parameter_in(x, (int32_t)f); }

extern "C"
void ossia_parameter_in_bang(t_parameter* x)
{ ossia_parameter_in(x, ossia::impulse{}); }

extern "C"
void ossia_parameter_in_symbol(t_parameter* x, t_symbol* f)
{ ossia_parameter_in(x, std::string(f->s_name)); }

extern "C"
void ossia_parameter_in_char(t_parameter* x, char f)
{ ossia_parameter_in(x, f); }

extern "C"
void ossia_parameter_in_anything(t_parameter* x, t_symbol *s, long argc, t_atom *argv)
{
    if (argc == 0)
    {
        ossia_parameter_in(x, std::string(s->s_name));
    }
    else
    {
        /* todo */
    }
}

namespace ossia {
    namespace max {
        
# pragma mark -
# pragma mark t_parameter structure functions
        
        bool t_parameter :: register_node(ossia::net::node_base* node)
        {
            bool res = do_registration(node);
            
            if (res)
            {
                object_dequarantining(this);
                
                for (auto remote : t_remote::quarantine().copy())
                    object_register<t_remote>(static_cast<t_remote*>(remote));
            }
            else
                object_quarantining(this);
            
            return res;
        }
        
        bool t_parameter :: do_registration(ossia::net::node_base* node)
        {
            if (m_node && m_node->get_parent() == node)
                return true; // already register to this node;
            
            unregister(); // we should unregister here because we may have add a node between the registered node and the parameter
            
            if (!node)
                return false;
            
            std::string absolute_path = object_path_absolute<t_parameter>(this);
            std::string address_string = ossia::net::address_string_from_node(*node);
            
            if (absolute_path != address_string)
                return false;
            
            m_node = &ossia::net::create_node(*node, m_name->s_name);
            if (m_node->get_name() != std::string(m_name->s_name))
                renaming(this);
            
            m_node->about_to_be_deleted.connect<t_parameter, &t_parameter::is_deleted>(this);
            
            ossia::net::address_base* localAddress{};
            
            if (m_type == gensym("float"))
            {
                localAddress = m_node->create_address(ossia::val_type::FLOAT);
                
                if (m_default[0].a_type == A_FLOAT)
                    ossia::net::set_default_value(localAddress->getNode(), (float)m_default[0].a_w.w_float);
            }
            else if (m_type == gensym("string"))
            {
                localAddress = m_node->create_address(ossia::val_type::STRING);
                
                if (m_default[0].a_type == A_SYM)
                    ossia::net::set_default_value(localAddress->getNode(), std::string(m_default[0].a_w.w_sym->s_name));
            }
            else if (m_type == gensym("int"))
            {
                localAddress = m_node->create_address(ossia::val_type::INT);
                
                if (m_default[0].a_type == A_LONG)
                    ossia::net::set_default_value(localAddress->getNode(), (int32_t)m_default[0].a_w.w_long);
            }
            else if (m_type == gensym("vec2f"))
            {
                localAddress = m_node->create_address(ossia::val_type::VEC2F);
                m_type_size = 2;
                
                if (m_default[0].a_type == A_FLOAT && m_default[1].a_type == A_FLOAT)
                {
                    vec2f vec = make_vec(m_default[0].a_w.w_float, m_default[1].a_w.w_float);
                    ossia::net::set_default_value(localAddress->getNode(), vec);
                }
            }
            else if (m_type == gensym("vec3f"))
            {
                localAddress = m_node->create_address(ossia::val_type::VEC3F);
                m_type_size = 3;
                
                if (m_default[0].a_type == A_FLOAT && m_default[1].a_type == A_FLOAT && m_default[2].a_type == A_FLOAT)
                {
                    vec3f vec = make_vec(m_default[0].a_w.w_float, m_default[1].a_w.w_float, m_default[2].a_w.w_float);
                    ossia::net::set_default_value(localAddress->getNode(), vec);
                }
            }
            else if (m_type == gensym("vec4f"))
            {
                localAddress = m_node->create_address(ossia::val_type::VEC4F);
                m_type_size = 4;
                
                if (m_default[0].a_type == A_FLOAT && m_default[1].a_type == A_FLOAT && m_default[2].a_type == A_FLOAT && m_default[3].a_type == A_FLOAT)
                {
                    vec4f vec = make_vec(m_default[0].a_w.w_float, m_default[1].a_w.w_float, m_default[2].a_w.w_float, m_default[3].a_w.w_float);
                    ossia::net::set_default_value(localAddress->getNode(), vec);
                }
            }
            else if (m_type == gensym("impulse"))
            {
                localAddress = m_node->create_address(ossia::val_type::IMPULSE);
                m_type_size = 0;
            }
            else if (m_type == gensym("bool"))
            {
                localAddress = m_node->create_address(ossia::val_type::BOOL);
                
                if (m_default[0].a_type == A_LONG)
                    ossia::net::set_default_value(localAddress->getNode(), m_default[0].a_w.w_long>0?true:false);
            }
            else if (m_type == gensym("tuple"))
            {
                localAddress = m_node->create_address(ossia::val_type::TUPLE);
                m_type_size = 64;
                std::vector<ossia::value> list;
                
                for ( int i = 0; i<64 && m_default[i].a_type != A_NOTHING; i++)
                {
                    if (m_default[i].a_type == A_FLOAT)
                        list.push_back(atom_getfloat(&m_default[i]));
                    else if (m_default[i].a_type == A_LONG)
                        list.push_back((int32_t)atom_getlong(&m_default[i]));
                    else if (m_default[i].a_type == A_SYM)
                        list.push_back(std::string(atom_getsym(&m_default[i])->s_name));
                }
                
                if (list.size() > 0) ossia::net::set_default_value(localAddress->getNode(), list);
            }
            else if (m_type == gensym("char"))
            {
                localAddress = m_node->create_address(ossia::val_type::CHAR);
                
                if (m_default[0].a_type == A_LONG)
                    ossia::net::set_default_value(localAddress->getNode(), (char)m_default[0].a_w.w_long);
            }
            else
            {
                object_error((t_object*)this, "type should one of (case sensitive): float, string, int, vec2f, vec3f, vec4f, bool, tuple, char");
            }
            
            if (!localAddress)
                return false;
            
            localAddress->set_domain(ossia::make_domain(atom_getfloat(&m_range[0]), atom_getfloat(&m_range[1])));
            
            // FIXME : we need case insensitive comparison here
            if (m_bounding_mode == gensym("FREE"))
                localAddress->set_bounding(ossia::bounding_mode::FREE);
            else if (m_bounding_mode == gensym("CLIP"))
                localAddress->set_bounding(ossia::bounding_mode::CLIP);
            else if (m_bounding_mode == gensym("WRAP"))
                localAddress->set_bounding(ossia::bounding_mode::WRAP);
            else if (m_bounding_mode == gensym("FOLD"))
                localAddress->set_bounding(ossia::bounding_mode::FOLD);
            else if (m_bounding_mode == gensym("LOW"))
                localAddress->set_bounding(ossia::bounding_mode::LOW);
            else if (m_bounding_mode == gensym("HIGH"))
                localAddress->set_bounding(ossia::bounding_mode::HIGH);
            
            if (m_access_mode == gensym("BI") || m_access_mode == gensym("RW"))
                localAddress->set_access(ossia::access_mode::BI);
            else if (m_access_mode == gensym("GET") || m_access_mode == gensym("R"))
                localAddress->set_access(ossia::access_mode::GET);
            else if (m_access_mode == gensym("SET") || m_access_mode == gensym("W"))
                localAddress->set_access(ossia::access_mode::SET);
            
            localAddress->set_repetition_filter(m_repetition_filter? ossia::repetition_filter::ON : ossia::repetition_filter::OFF);
            
            ossia::unit_t unit = ossia::parse_pretty_unit(m_unit->s_name);
            localAddress->set_unit(unit);
            
            ossia::net::set_description(localAddress->getNode(), m_description->s_name);
            ossia::net::set_tags(localAddress->getNode(), parse_tags_symbol(m_tags));
            
            ossia::net::set_priority(localAddress->getNode(), m_priority);
            
            localAddress->add_callback([=](const ossia::value& v) { apply_value_visitor(v); });
            
            clock_delay(m_clock, 0);
            
            return true;
        }
        
        bool t_parameter :: unregister()
        {
            if (m_node)
            {
                if (m_node->get_parent())
                    m_node->get_parent()->remove_child(*m_node);
                
                m_node = nullptr;
                
                for (auto remote : t_remote::quarantine().copy())
                    object_register<t_remote>(static_cast<t_remote*>(remote));
                
            }
            
            object_quarantining(this);
            derenaming(this);
            
            for (auto parameter : t_parameter::rename().copy())
            {
                if (strcmp(parameter->m_name->s_name, m_name->s_name) == 0)
                {
                    parameter->unregister();
                    object_register<t_parameter>(parameter);
                }
            }
            
            return true;
        }
        
        void t_parameter :: is_deleted(const ossia::net::node_base& n)
        {
            m_node->about_to_be_deleted.disconnect<t_parameter, &t_parameter::is_deleted>(this);
            m_node = nullptr;
            object_quarantining<t_parameter>(this);
        }
        
        bool t_parameter :: is_renamed(t_parameter* x)
        {
            return x->rename().contains(x);
        }
        
        void t_parameter :: renaming(t_parameter* x)
        {
            if (!is_renamed(x)) x->rename().push_back(x);
        }
        
        void t_parameter :: derenaming(t_parameter* x)
        {
            x->rename().remove_all(x);
        }

        ossia::safe_vector<t_parameter *> &t_parameter::quarantine()
        {
            static ossia::safe_vector<t_parameter*> quarantine;
            return quarantine;
        }

        ossia::safe_vector<t_parameter *> &t_parameter::rename()
        {
            static ossia::safe_vector<t_parameter*> rename;
            return rename;
        }
        
    } // max namespace
} // ossia namespace
