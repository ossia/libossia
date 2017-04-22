#include "device.hpp"
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "ossia/network/oscquery/oscquery_server.hpp"
#include "ossia/network/osc/osc.hpp"

namespace ossia { namespace pd {

static t_eclass *device_class;

static void *device_new(t_symbol *name, int argc, t_atom *argv)
{
    t_device *x = (t_device *)eobj_new(device_class);
    // TODO SANITIZE : Direct leak
    t_binbuf* d = binbuf_via_atoms(argc,argv);

    if(x && d)
    {
        x->x_name = gensym("Pd");
        x->x_dumpout = outlet_new((t_object*)x,gensym("dumpout"));

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
        }

        auto local_proto_ptr = std::make_unique<ossia::net::local_protocol>();
        x->x_device = new ossia::net::generic_device{std::move(local_proto_ptr), x->x_name->s_name};
        x->x_node = &x->x_device->get_root_node();

        ebox_attrprocess_viabinbuf(x, d);
    }

    return (x);
}

static void device_free(t_device *x)
{
    x->x_dead = true;
    x->unregister_children();
    if (x->x_device) delete(x->x_device);
}

static void dump_child(t_device *x, const ossia::net::node_base& node){
    for (const auto& child : node.children()){
        std::stringstream ss;
        auto parent = child->get_parent();
        while (parent != nullptr)
        {
            ss << "\t";
            parent = parent->get_parent();
        }
        ss << child->get_name();
        t_atom a;
        SETSYMBOL(&a,gensym(ss.str().c_str()));
        outlet_anything(x->x_dumpout, gensym("child"), 1, &a);
        dump_child(x, *child);
    }
}

static void device_dump(t_device *x){
    dump_child(x, x->x_device->get_root_node());
}

void t_device :: loadbang(t_device* x, t_float type){
    if (LB_LOAD == (int)type){
        register_children(x);
    }
}

void t_device :: register_children(t_device* x){

    std::vector<obj_hierachy> modelnodes = find_child_to_register(x, x->x_obj.o_canvas->gl_list, "ossia.model");
    for (auto v : modelnodes){
        if(v.classname == "ossia.model"){
            t_model* model = (t_model*) v.x;
            model->register_node(x->x_node);
        } else if(v.classname == "ossia.param"){
            t_param* param = (t_param*) v.x;
            param->register_node(x->x_node);
        }
    }

    std::vector<obj_hierachy> viewnodes = find_child_to_register(x, x->x_obj.o_canvas->gl_list, "ossia.view");
    for (auto v : viewnodes){
        if(v.classname == "ossia.view"){
            t_view* view = (t_view*) v.x;
            view->register_node(x->x_node);
        } else if(v.classname == "ossia.remote"){
            t_remote* remote = (t_remote*) v.x;
            remote->register_node(x->x_node);
        }
    }
}

void t_device :: unregister_children(){
    std::vector<obj_hierachy> node = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.model");
    for (auto v : node){
        if(v.classname == "ossia.model"){
            t_model* model = (t_model*) v.x;
            model->unregister();
        } else if(v.classname == "ossia.param"){
            t_param* param = (t_param*) v.x;
            param->unregister();
        }
    }

    std::vector<obj_hierachy> viewnode = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.view");
    for (auto v : viewnode){
        if(v.classname == "ossia.view"){
            t_view* view = (t_view*) v.x;
            view->unregister();
        } else if(v.classname == "ossia.remote"){
            t_remote* remote = (t_remote*) v.x;
            remote->unregister();
        }
    }
}

static void device_expose(t_device* x, t_symbol*, int argc, t_atom* argv){

    if (argc && argv->a_type == A_SYMBOL){
        auto& proto = static_cast<ossia::net::local_protocol&>(x->x_device->get_protocol());
        std::string protocol = argv->a_w.w_symbol->s_name;
        if (protocol == "Minuit"){
            Protocol_Settings::minuit settings{};
            argc--;
            argv++;
            if ( argc == 3
                 && argv[0].a_type == A_SYMBOL
                 && argv[1].a_type == A_FLOAT
                 && argv[2].a_type == A_FLOAT)
            {
                settings.remoteip = atom_getsymbol(argv++)->s_name;
                settings.remoteport = atom_getfloat(argv++);
                settings.localport = atom_getfloat(argv++);
            }

            try {
                proto.expose_to(std::make_unique<ossia::net::minuit_protocol>(x->x_name->s_name, settings.remoteip, settings.remoteport, settings.localport));
            } catch (const std::exception& e) {
                pd_error(x,"%s",e.what());
                return;
            }
            logpost(x,3,"New 'Minuit' protocol connected to %s on port %u and listening on port %u",  settings.remoteip.c_str(), settings.remoteport, settings.localport);
        }
        else if (protocol == "oscquery"){
            Protocol_Settings::oscquery settings{};
            argc--;
            argv++;
            if ( argc == 2
                 && argv[0].a_type == A_FLOAT
                 && argv[1].a_type == A_FLOAT)
            {
                settings.oscport = atom_getfloat(argv++);
                settings.wsport = atom_getfloat(argv++);
            }

            try{
                proto.expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>(settings.oscport, settings.wsport));
            } catch (const std::exception& e) {
                pd_error(x,"%s",e.what());
                return;
            }
            logpost(x,3,"New 'oscquery' protocol with OSC port %u and WS port %u, listening on port %u",  settings.oscport, settings.wsport, settings.oscport);
        } else if (protocol == "osc"){
            Protocol_Settings::osc settings{};
            argc--;
            argv++;
            if ( argc == 3
                 && argv[0].a_type == A_SYMBOL
                 && argv[1].a_type == A_FLOAT
                 && argv[2].a_type == A_FLOAT)
            {
                settings.remoteip = atom_getsymbol(argv)->s_name;
                settings.remoteport = atom_getfloat(argv++);
                settings.localport = atom_getfloat(argv++);
            }

            try{
                proto.expose_to(std::make_unique<ossia::net::osc_protocol>(settings.remoteip, settings.remoteport , settings.localport));
            } catch (const std::exception& e) {
                pd_error(x,"%s",e.what());
                return;
            }
            logpost(x,3,"New 'OSC' protocol connect to %s on port %u and listening on port %u",  settings.remoteip.c_str(), settings.remoteport, settings.localport);
        } else {
            pd_error((t_object*)x, "Unknown protocol: %s", protocol.c_str());
        }
    } else Protocol_Settings::print_protocol_help();
}

extern "C" void setup_ossia0x2edevice(void)
{
    t_eclass *c = eclass_new("ossia.device", (method)device_new, (method)device_free, (short)sizeof(t_device), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) t_device::register_children, "register", A_NULL, 0);
        eclass_addmethod(c, (method) t_device::loadbang, "loadbang", A_NULL, 0);
        eclass_addmethod(c, (method) device_dump, "dump", A_NULL, 0);
        eclass_addmethod(c, (method) device_expose, "expose", A_GIMME, 0);
        eclass_addmethod(c, (method) Protocol_Settings::print_protocol_help, "help", A_NULL, 0);
    }

    device_class = c;
}


} } //namespace
