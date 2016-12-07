#include "parameter.hpp"
#include "device.hpp"
#include "model.hpp"
#include <limits>

static t_eclass *parameter_class;

static void parameter_free(t_param* x);

void t_param::setValue(const ossia::value& v){
    value_visitor<t_param> vm;
    vm.x = (t_param*) &x_obj;
    v.apply(vm);
}

static void parameter_set(t_param *x, t_symbol* s, int argc, t_atom* argv){
    if ( x->x_localAddress ){
        while(argc--){
            switch(argv->a_type){
            case A_FLOAT:
                x->x_localAddress->pushValue(float(argv->a_w.w_float));
                break;
            case A_SYMBOL:
            {
                x->x_localAddress->pushValue(std::string(argv->a_w.w_symbol->s_name));
                break;
            }
            default:
                pd_error(x,"atom type %d is not supported", argv->a_type);
            }
            argv++;
        }
    }
}

static void parameter_dump(t_model *x)
{
    t_atom a;
    std::string fullpath = get_absolute_path(x->x_node);
    SETSYMBOL(&a,gensym(fullpath.c_str()));
    outlet_anything(x->dumpout,gensym("fullpath"), 1, &a);
}

static void parameter_bang(t_param *x){
    if ( x->x_localAddress ) x->x_localAddress->pullValue();
}

void parameter_loadbang(t_param* x){
    std::cout << "[ossia.parameter] loadbang" << std::endl;
    obj_register<t_param>(x);
}

bool t_param :: register_node(ossia::net::node_base* node){

    if (x_node && x_node->getParent() == node ) return true; // already register to this node;

    unregister(); // we should unregister here because we may have add a node between the registered node and the parameter

    std::cout << "[ossia.param] : register parameter : " << x_name->s_name << std::endl;

    if(node){
        std::cout << "[ossia.param] :  x->x_node->children.size() : " << node->children().size() << std::endl;
        for (const auto& child : node->children()){
            if(child->getName() == x_name->s_name){
                pd_error(this, "a parameter with adress '%s' already exists.", x_name->s_name);
                return false;
            }
        }
        std::cout << "create node :  " << x_name->s_name << std::endl;
        x_node = node->createChild(x_name->s_name);
        if(x_type == gensym("symbol")){
            x_localAddress = x_node->createAddress(ossia::val_type::STRING);
        } else {
            x_localAddress = x_node->createAddress(ossia::val_type::FLOAT);
            //TODO : set domain x_localAddress->setDomain()
        }
        x_localAddress->add_callback([=](const ossia::value& v){
            setValue(v);
        });
        if (x_default.a_type != A_NULL){
            parameter_set(this,gensym("set"),1,&x_default);
        }
    } else {
        return false;
    }

    return true;
}

bool t_param :: unregister(){
    std::cout << "[ossia.param] unregister parameter : " << x_name->s_name << std::endl;
    if (x_node) {
        x_node->getParent()->removeChild(x_name->s_name);
        x_node = nullptr;
        x_localAddress = nullptr;
    }
    return true;
}

static void parameter_float(t_param *x, t_float val){
    if ( x->x_localAddress ){
        x->x_localAddress->pushValue(float(val));
    }
}

static void *parameter_new(t_symbol *name, int argc, t_atom *argv)
{
    t_param *x = (t_param *)eobj_new(parameter_class);

    std::cout << "[ossia.parameter] new instance: " << std::hex << x << std::endl;

    t_binbuf* d = binbuf_via_atoms(argc,argv);

    if(x && d)
    {
        x->x_absolute = false;
        x->x_node = nullptr;
        x->x_localAddress = nullptr;
        x->range[0] = std::numeric_limits<float>::min();
        x->range[1] = std::numeric_limits<float>::max();

        x->x_setout  = outlet_new((t_object*)x,nullptr);
        x->x_dataout = outlet_new((t_object*)x,nullptr);
        x->x_dumpout = outlet_new((t_object*)x,gensym("dumpout"));

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
            if (x->x_name != osym_empty && x->x_name->s_name[0] == '/') x->x_absolute = true;

            // if we only pass a default value without setting parameter type,
            // the type is deduced from the default value (for now in Pd only symbol and float)
            if(x->x_default.a_type == A_SYMBOL) x->x_type = gensym("symbol");
        } else {
            error("You have to pass a name as the first argument");
            x->x_name=gensym("untitledParam");
        }

        ebox_attrprocess_viabinbuf(x, d);
    }

    return (x);
}

static void parameter_free(t_param *x)
{
    x->unregister();
    outlet_free(x->x_dataout);
    outlet_free(x->x_setout);
    outlet_free(x->x_dumpout);
}

extern "C" void setup_ossia0x2eparam(void)
{
    t_eclass *c = eclass_new("ossia.param", (method)parameter_new, (method)parameter_free, (short)sizeof(t_param), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) parameter_loadbang,   "loadbang",   A_NULL, 0);
        eclass_addmethod(c, (method) parameter_float,      "float",      A_FLOAT, 0);
        eclass_addmethod(c, (method) parameter_set,        "set",        A_GIMME, 0);
        eclass_addmethod(c, (method) parameter_bang,       "bang",       A_NULL, 0);
        eclass_addmethod(c, (method) parameter_dump,       "dump",       A_NULL, 0);

        CLASS_ATTR_SYMBOL(c, "type",    1, t_param, x_type);
        CLASS_ATTR_DEFAULT(c, "type",0,"float");
        CLASS_ATTR_ATOM  (c, "default", 1, t_param, x_default);
        CLASS_ATTR_FLOAT_ARRAY(c, "range", 0, t_param, range, 2);
        CLASS_ATTR_FLOAT(c, "min", 0, t_param, range);
        // CLASS_ATTR_FLOAT(c, "max", 0, t_parameter, range+1);
        eclass_new_attr_typed(c,"max", "float", 1, 0, 0, calcoffset(t_param,range)+sizeof(float));

        eclass_register(CLASS_OBJ, c);

    }

    parameter_class = c;
}
