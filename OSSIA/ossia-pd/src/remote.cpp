#include "remote.hpp"
#include "device.hpp"
#include "parameter.hpp"

static t_eclass *remote_class;

static void remote_free(t_remote* x);

void t_remote::setValue(const ossia::value& v){
    value_visitor<t_remote> vm;
    vm.x = (t_remote*) &x_obj;
    v.apply(vm);
}

static void remote_set(t_remote *x, t_symbol* s, int argc, t_atom* argv){
    if ( x->x_node && x->x_node->getAddress() ){
        while(argc--){
            switch(argv->a_type){
            case A_FLOAT:
                x->x_node->getAddress()->pushValue(float(argv->a_w.w_float));
                break;
            case A_SYMBOL:
            {
                // FIXME : this call operator()(Char c) instead of operator()(const String& s)
                x->x_node->getAddress()->pushValue(std::string(argv->a_w.w_symbol->s_name));
                break;
            }
            default:
                pd_error(x,"atom type %d is not supported", argv->a_type);
            }
            argv++;
        }
    } else {
        pd_error(x,"[ossia.remote %s] is not registered to any parameter",x->x_name->s_name);
    }
}

static void remote_bang(t_remote *x){
    if ( x->x_node->getAddress() ) x->x_node->getAddress()->pullValue();
}

static void remote_loadbang(t_remote *x){
    std::cout << "[ossia.remote] loadbang" << std::endl;
    if (obj_register<t_remote>(x)) remote_bang(x); // if correctly registered then pull the value
}

bool t_remote :: register_node(ossia::net::node_base* node){
    std::cout << "register remote : " << x_name->s_name << std::endl;

    if (x_node && x_node->getParent() == node ) return true; // already register to this node;

    unregister(); // we should unregister here because we may have add a node between the registered node and the parameter

    if (!node){
        std::cout << "t_remote :: register(node) : invalid node" << std::endl;
        return false;
    }

    if(node){
        for (const auto& child : node->children()){
            if(child->getName() == x_name->s_name){
                x_node = child.get();
                x_callbackit = x_node->getAddress()->add_callback([=](const ossia::value& v){
                    setValue(v);
                });
                return true;
            }
        }
    }
    return false;
}

static void remote_unregister(t_remote* x, t_symbol* s, void* ptr){
    ossia::net::node_base* node = (ossia::net::node_base*) ptr;
    if ( node == x->x_node ){
        x->x_node = nullptr;
        std::cout << "t_remote " << x->x_name->s_name << " invalidate x_node pointer" << std::endl;
    }
}

bool t_remote :: unregister(){
    std::cout << "unregister remote : " << x_name->s_name << std::endl;

    if (x_callbackit != boost::none) {
        // we have to remove the callback, but assigning x_callbackit to dummy_list.end(); seems weird to me..., have to think about a better solution
        x_node->getAddress()->remove_callback(*x_callbackit);
        x_callbackit = boost::none;
    }
    x_node = nullptr;
    return true;
}

static void remote_float(t_remote *x, t_float val){
    if ( x->x_node && x->x_node->getAddress() ){
        x->x_node->getAddress()->pushValue(float(val));
    } else {
        pd_error(x,"[ossia.remote %s] is not registered to any parameter", x->x_name->s_name);
    }
}

static void *remote_new(t_symbol *name, int argc, t_atom *argv)
{
    t_remote *x = (t_remote *)eobj_new(remote_class);

    if(x)
    {
        x->x_absolute = false;
        x->x_node = nullptr;
        x->x_setout = outlet_new((t_object*)x, nullptr);
        x->x_dataout = outlet_new((t_object*)x,nullptr);
        x->x_dumpout = outlet_new((t_object*)x,gensym("dumpout"));
        x->x_callbackit = boost::none;
        t_pd * a;
        pd_bind((t_pd*)x, osym_send_remote );

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
            if (x->x_name != osym_empty && x->x_name->s_name[0] == '/') x->x_absolute = true;

        } else {
            error("You have to pass a name as the first argument");
            x->x_name = gensym("untitledRemote");
        }

        ;
    }

    return (x);
}

static void remote_free(t_remote *x)
{
    x->unregister();
    outlet_free(x->x_setout);
    outlet_free(x->x_dataout);
    outlet_free(x->x_dumpout);
}

extern "C" void setup_ossia0x2eremote(void)
{
    t_eclass *c = eclass_new("ossia.remote", (method)remote_new, (method)remote_free, (short)sizeof(t_remote), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) remote_loadbang,   "loadbang",   A_NULL, 0);
        eclass_addmethod(c, (method) remote_float,      "float",      A_FLOAT, 0);
        eclass_addmethod(c, (method) remote_set,        "set",        A_GIMME, 0);
        eclass_addmethod(c, (method) remote_bang,       "bang",       A_NULL, 0);
        eclass_addmethod(c, (method) remote_unregister,       "unregister", A_POINTER, 0);
    }

    remote_class = c;
}
