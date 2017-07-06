#include "device.hpp"
#include "model.hpp"
#include "parameter.hpp"
//#include "remote.hpp"
//#include "view.hpp"

#include "ossia/network/oscquery/oscquery_server.hpp"
#include "ossia/network/osc/osc.hpp"

using namespace ossia::max;
    
# pragma mark -
# pragma mark ossia_device class methods
    
extern "C"
void ossia_device_setup(void)
{
    auto& ossia_library = ossia_max::instance();
    
    // instantiate the ossia.client class
    ossia_library.ossia_device_class = class_new("ossia.device",
                                                 (method)ossia_device_new,
                                                 (method)ossia_device_free,
                                                 (short)sizeof(t_device),
                                                 0L, A_GIMME, 0);
    
    class_addmethod(ossia_library.ossia_device_class, (method)t_device::register_children,              "register",     A_NOTHING,  0);
    class_addmethod(ossia_library.ossia_device_class, (method)t_device::loadbang,                       "loadbang",     A_NOTHING,  0);
    class_addmethod(ossia_library.ossia_device_class, (method)ossia_device_dump,                        "dump",         A_NOTHING,  0);
    class_addmethod(ossia_library.ossia_device_class, (method)ossia_device_expose,                      "expose",       A_GIMME,    0);
    class_addmethod(ossia_library.ossia_device_class, (method)protocol_settings::print_protocol_help,   "help",         A_NOTHING,  0);
    
    class_register(CLASS_BOX, ossia_library.ossia_device_class);
}

extern "C"
void *ossia_device_new(t_symbol *name, long argc, t_atom *argv)
{
    auto& ossia_library = ossia_max::instance();
    t_device* x = (t_device*) object_alloc(ossia_library.ossia_device_class);
    
    if (x)
    {
        // make outlets
        x->m_dump_out = outlet_new(x, NULL);						// anything outlet to dump client state
        
        // parse attributes
        long attrstart = attr_args_offset(argc, argv);
        
        // check name argument
        x->m_name = gensym("Max");
        if (attrstart && argv)
        {
            if (atom_gettype(argv) == A_SYM)
            {
                x->m_name = atom_getsym(argv);
            }
        }
        
        // TODO : move local protocol creation into ossia Library loading to do this only one time
        // TODO : only allow local device renaming here
        auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();
        x->m_device = new ossia::net::generic_device{std::move(local_proto_ptr), x->m_name->s_name};
        
        x->m_node = &x->m_device->get_root_node();
    }
    
    return (x);
}

extern "C"
void ossia_device_free(t_device *x)
{
    x->m_dead = true;
    x->unregister_children();
    if (x->m_device)
        delete(x->m_device);
}

static void dump_child(t_device *x, const ossia::net::node_base& node)
{
    for (const auto& child : node.children())
    {
        std::stringstream ss;
        auto parent = child->get_parent();
        
        while (parent != nullptr)
        {
            ss << "\t";
            parent = parent->get_parent();
        }
        ss << child->get_name();
        
        t_atom a;
        atom_setsym(&a,gensym(ss.str().c_str()));
        outlet_anything(x->m_dump_out, gensym("child"), 1, &a);
        dump_child(x, *child);
    }
}

extern "C"
void ossia_device_dump(t_device *x)
{
    dump_child(x, x->m_device->get_root_node());
}

extern "C"
void ossia_device_expose(t_device* x, t_symbol*, long argc, t_atom* argv)
{
    if (argc && argv->a_type == A_SYM)
    {
        auto& proto = static_cast<ossia::net::local_protocol&>(x->m_device->get_protocol());
        std::string protocol = atom_getsym(argv)->s_name;
        
        if (protocol == "Minuit")
        {
            protocol_settings::minuit settings{};
            
            argc--;
            argv++;
            
            if (argc == 3
                && argv[0].a_type == A_SYM
                && argv[1].a_type == A_FLOAT
                && argv[2].a_type == A_FLOAT)
            {
                settings.remoteip = atom_getsym(argv++)->s_name;
                settings.remoteport = atom_getfloat(argv++);
                settings.localport = atom_getfloat(argv++);
            }
            
            try
            {
                proto.expose_to(std::make_unique<ossia::net::minuit_protocol>(x->m_name->s_name, settings.remoteip, settings.remoteport, settings.localport));
            }
            catch (const std::exception& e)
            {
                object_error((t_object*)x, "%s", e.what());
                return;
            }
            
            object_post((t_object*)x,"Connected with Minuit protocol to %s on port %u and listening on port %u",  settings.remoteip.c_str(), settings.remoteport, settings.localport);
        }
        else if (protocol == "oscquery")
        {
            protocol_settings::oscquery settings{};
            
            argc--;
            argv++;
            
            if (argc == 2
                && argv[0].a_type == A_FLOAT
                && argv[1].a_type == A_FLOAT)
            {
                settings.oscport = atom_getfloat(argv++);
                settings.wsport = atom_getfloat(argv++);
            }
            
            try
            {
                proto.expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>(settings.oscport, settings.wsport));
            }
            catch (const std::exception& e)
            {
                object_error((t_object*)x, "%s", e.what());
                return;
            }
            
            object_post((t_object*)x, "Connected with oscquery protocol with OSC port %u and WS port %u, listening on port %u",  settings.oscport, settings.wsport, settings.oscport);
            
        }
        else if (protocol == "osc")
        {
            protocol_settings::osc settings{};
            
            argc--;
            argv++;
            
            if (argc == 3
                && argv[0].a_type == A_SYM
                && argv[1].a_type == A_FLOAT
                && argv[2].a_type == A_FLOAT)
            {
                settings.remoteip = atom_getsym(argv)->s_name;
                settings.remoteport = atom_getfloat(argv++);
                settings.localport = atom_getfloat(argv++);
            }
            
            try
            {
                proto.expose_to(std::make_unique<ossia::net::osc_protocol>(settings.remoteip, settings.remoteport , settings.localport));
            }
            catch (const std::exception& e)
            {
                object_error((t_object*)x, "%s", e.what());
                return;
            }
            
            object_post((t_object*)x, "Connected with OSC protocol to %s on port %u and listening on port %u", settings.remoteip.c_str(), settings.remoteport, settings.localport);
        }
        else
        {
            object_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
        }
    }
    else protocol_settings::print_protocol_help();
}

namespace ossia {
namespace max {

# pragma mark -
# pragma mark t_device structure functions
    
    void t_device :: register_children(t_device* x)
    {
        std::vector<box_hierachy> children_model = find_children_to_register(&x->m_object, get_patcher(&x->m_object), gensym("ossia.model"));
        
        for (auto child : children_model)
        {
            if (child.classname == gensym("ossia.model"))
            {
                t_model* model = (t_model*)jbox_get_object(child.box);
                model->register_node(x->m_node);
            }
            else if (child.classname == gensym("ossia.parameter"))
            {
                t_parameter* parameter = (t_parameter*)jbox_get_object(child.box);
                parameter->register_node(x->m_node);
            }
        }
        
        std::vector<box_hierachy> children_view = find_children_to_register(&x->m_object, get_patcher(&x->m_object), gensym("ossia.view"));
        
        for (auto v : children_view)
        {
            if (v.classname == gensym("ossia.view"))
            {
//                t_view* view = (t_view*)jbox_get_object(child.box);
//                view->register_node(x->m_node);
            }
            else if (v.classname == gensym("ossia.remote"))
            {
//                t_remote* remote = (t_remote*)jbox_get_object(child.box);
//                remote->register_node(x->m_node);
            }
        }
    }
    
    void t_device :: unregister_children()
    {
        std::vector<box_hierachy> children_model = find_children_to_register(&m_object, get_patcher(&m_object), gensym("ossia.model"));
        
        for (auto child : children_model)
        {
            if (child.classname == gensym("ossia.model"))
            {
                t_model* model = (t_model*)jbox_get_object(child.box);
                model->unregister();
            }
            else if (child.classname == gensym("ossia.param"))
            {
                t_parameter* parameter = (t_parameter*)jbox_get_object(child.box);
                parameter->unregister();
            }
        }
        
        std::vector<box_hierachy> children_view = find_children_to_register(&m_object, get_patcher(&m_object), gensym("ossia.view"));
        
        for (auto child : children_view)
        {
            if (child.classname == gensym("ossia.view"))
            {
//                t_view* view = (t_view*)jbox_get_object(child.box);
//                view->unregister();
            }
            else if (child.classname == gensym("ossia.remote"))
            {
//                t_remote* remote = (t_remote*)jbox_get_object(child.box);
//                remote->unregister();
            }
        }
    }
    
    void t_device :: loadbang(t_device* x)
    {
        //if (LB_LOAD == (int)type)
        register_children(x);
    }
    
} // max namespace
} // ossia namespace
