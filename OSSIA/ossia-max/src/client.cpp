#include "client.hpp"
//#include "model.hpp"
#include "parameter.hpp"
//#include "remote.hpp"
//#include "view.hpp"
#include <ossia/network/oscquery/oscquery_mirror.hpp>
#include "ossia/network/osc/osc.hpp"

#include <iostream>
#include <memory>
#include <functional>

using namespace ossia::max;

# pragma mark -
# pragma mark ossia_parameter class methods

extern "C"
void ossia_client_setup(void)
{
    auto& ossia_library = ossia_max::instance();
    
    // instantiate the ossia.client class
    ossia_library.ossia_client_class = class_new("ossia.client",
                                                 (method)ossia_client_new,
                                                 (method)ossia_client_free,
                                                 (short)sizeof(t_client),
                                                 0L, A_GIMME, 0);
    
    class_addmethod(ossia_library.ossia_client_class, (method)t_client::register_children,      "register",     A_NOTHING,  0);
    class_addmethod(ossia_library.ossia_client_class, (method)ossia_client_loadbang,            "loadbang",     A_NOTHING,  0);
    class_addmethod(ossia_library.ossia_client_class, (method)ossia_client_dump,                "dump",         A_NOTHING,  0);
    class_addmethod(ossia_library.ossia_client_class, (method)ossia_client_connect,             "connect",      A_GIMME,    0);
    class_addmethod(ossia_library.ossia_client_class, (method)ossia_client_print_protocol_help, "help",         A_NOTHING,  0);
    
    class_register(CLASS_BOX, ossia_library.ossia_client_class);
}

extern "C"
void *ossia_client_new(t_symbol* name, long argc, t_atom* argv)
{
    auto& ossia_library = ossia_max::instance();
    t_client* x = (t_client *) object_alloc(ossia_library.ossia_client_class);
    
    if (x)
    {
        // make outlets
        x->m_dump_out = outlet_new(x, NULL);						// anything outlet to dump client state

        x->m_device = 0;
        x->m_node = 0;
        
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
    }

    return (x);
}

extern "C"
void ossia_client_free(t_client *x)
{
    x->m_dead = true;
    x->unregister_children();
    
    if (x->m_device)
        delete(x->m_device);
    
    x->m_device = nullptr;
}

extern "C"
void ossia_client_loadbang(t_client* x)
{
    //if (LB_LOAD == (int)type)
    t_client::register_children(x);
}

static void dump_child(t_client *x, const ossia::net::node_base& node)
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
        atom_setsym(&a, gensym(ss.str().c_str()));
        outlet_anything(x->m_dump_out, gensym("child"), 1, &a);
        dump_child(x, *child);
    }
}

extern "C"
void ossia_client_dump(t_client *x)
{
    dump_child(x, x->m_device->get_root_node());
}

extern "C"
void ossia_client_connect(t_client* x, t_symbol*, int argc, t_atom* argv)
{
    ossia_client_free(x); // unregister and delete m_device
    
    if (argc && argv->a_type == A_SYM)
    {
        std::string protocol = atom_getsym(argv)->s_name;
        
        if (protocol == "Minuit")
        {
            std::string device_name = "Max";
            std::string remoteip = "127.0.0.1";
            int remoteport = 6666;
            int localport = 9999;
            
            argc--;
            argv++;
            
            if ( argc == 4
                && argv[0].a_type == A_SYM
                && argv[1].a_type == A_SYM
                && argv[2].a_type == A_FLOAT
                && argv[3].a_type == A_FLOAT)
            {
                device_name = atom_getsym(argv++)->s_name;
                remoteip = atom_getsym(argv++)->s_name;
                remoteport = atom_getfloat(argv++);
                localport = atom_getfloat(argv++);
            }
            
            try
            {
                x->m_device = new ossia::net::generic_device
                { std::make_unique<ossia::net::minuit_protocol>(device_name, remoteip, remoteport, localport), x->m_name->s_name };
            }
            catch (const std::exception& e)
            {
                object_error((t_object*)x, "%s", e.what());
                return;
            }
            
            object_post((t_object*)x, "Connected with Minuit protocol to %s");
        }
        else if (protocol == "oscquery")
        {
            argc--;
            argv++;
            std::string wsurl = "ws://127.0.0.1:5678";
            
            if ( argc == 1 && argv[0].a_type == A_SYM)
            {
                wsurl = atom_getsym(argv)->s_name;
            }
            
            try
            {
                auto protocol = new ossia::oscquery::oscquery_mirror_protocol{wsurl};
                x->m_device = new ossia::net::generic_device{std::unique_ptr<ossia::net::protocol_base>(protocol), "Pd"};
                
                if (x->m_device)
                    std::cout << "connected to device " << x->m_device->get_name() << " on " << wsurl << std::endl;
            }
            catch (const std::exception&  e)
            {
                object_error((t_object*)x, "%s", e.what());
                return;
            }
            
            object_post((t_object*)x, "Connected with oscquery protocol to %s", wsurl.c_str());
        }
        else
        {
            object_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
            return;
        }
    }
    else
    {
        ossia_client_print_protocol_help(x);
        return;
    }
    
    if (x->m_device)
    {
        x->m_device->get_protocol().update(*x->m_device);
        x->m_node = &x->m_device->get_root_node();
        
        t_client::register_children(x);
        t_client::explore(x->m_device->get_root_node());
        
        for (auto& child : x->m_device->children())
        {
            std::cout << child->get_name() << std::endl;
        }
    }
}

extern "C"
void ossia_client_print_protocol_help(t_client *x)
{
    object_post((t_object*)x, "connect <protocol> <args> ...");
    object_post((t_object*)x, "Available protocols (case sensitive): Minuit, oscquery");
    object_post((t_object*)x, "Protocols parameters :");
    object_post((t_object*)x, "Minuit <remoteip> <remoteport> <localport> :\n");
    object_post((t_object*)x, "\tremoteip (symbol): ip of target device\n");
    object_post((t_object*)x, "\tremoteport (float): port on which packet should be send\n");
    object_post((t_object*)x, "\tlocalport (float): port to which this device is listening");
    object_post((t_object*)x, "oscquery <oscport> <wsurl> :\n");
    object_post((t_object*)x, "\twsurl (symbol) : url to connect to (default : ws://127.0.0.1:5678)\n");
}

namespace ossia {
namespace max {
    
# pragma mark -
# pragma mark t_parameter structure functions
    
    void t_client :: register_children(t_client* x)
    {
        //std::vector<object_hierachy> viewnodes = find_child_to_register(x, x->m_object.o_canvas->gl_list, "ossia.view");
        /*
        for (auto v : viewnodes)
        {
            if (v.classname == "ossia.view")
            {
                t_view* view = (t_view*) v.x;
                view->register_node(x->m_node);
            }
            else if (v.classname == "ossia.remote")
            {
                t_remote* remote = (t_remote*) v.x;
                remote->register_node(x->m_node);
            }
        }
         */
    }
    
    void t_client :: unregister_children()
    {
        //std::vector<object_hierachy> node = find_child_to_register(this, m_obj.o_canvas->gl_list, "ossia.model");
        /*
        for (auto v : node)
        {
            if (v.classname == "ossia.model")
            {
                t_model* model = (t_model*) v.x;
                model->unregister();
            }
            else if (v.classname == "ossia.param")
            {
                t_param* param = (t_param*) v.x;
                param->unregister();
            }
        }
        */
        //std::vector<obj_hierachy> viewnode = find_child_to_register(this, m_obj.o_canvas->gl_list, "ossia.view");
        /*
        for (auto v : viewnode)
        {
            if (v.classname == "ossia.view")
            {
                t_view* view = (t_view*) v.x;
                view->unregister();
            }
            else if (v.classname == "ossia.remote")
            {
                t_remote* remote = (t_remote*) v.x;
                remote->unregister();
            }
        }
         */
    }
    
    void t_client :: explore(const ossia::net::node_base& node)
    {
        for (const auto& child : node.children_copy())
        {
            if (auto addr = child->get_address())
            {
                // attach to callback to display value update
                addr->add_callback([=] (const value& v) {
                    std::cerr << "[message] " << osc_address_string(*addr)
                    << " <- " <<  value_to_pretty_string(v) << std::endl;
                });
                
                // update the value
                addr->pull_value();
            }
            
            /*
             fmt::MemoryWriter w;
             w << *child;
             std::cerr << w.str() << "\n";
             */
            std::cout << child->get_name() <<  std::endl;
            
            explore(*child);
        }
    }

} // max namespace
} // ossia namespace
