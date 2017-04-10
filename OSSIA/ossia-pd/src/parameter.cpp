#include "parameter.hpp"
#include "device.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "ossia/editor/dataspace/dataspace_visitors.hpp"
#include <limits>

namespace ossia { namespace pd {

static t_eclass *parameter_class;

static void parameter_free(t_param* x);

bool t_param :: register_node(ossia::net::node_base* node){
    bool res = do_registration(node);
    if (res) {
        obj_dequarantining<t_param>(this);
        for (auto remote : t_remote::quarantine()){
            obj_register<t_remote>(static_cast<t_remote*>(remote));
        }
    } else obj_quarantining<t_param>(this);

    return res;
}

static void push_default_value(t_param* x){
    int i = 0;
    for (; i<x->x_type_size ; i++){
        if (x->x_default[i].a_type == A_NULL) break;
    }
    t_obj_base::obj_push(x,nullptr,i,x->x_default);
}

bool t_param :: do_registration(ossia::net::node_base* node){

    if (x_node && x_node->getParent() == node ) return true; // already register to this node;

    unregister(); // we should unregister here because we may have add a node between the registered node and the parameter

    if(!node) return false;

    std::string absolute_path = get_absolute_path<t_param>(this);
    std::string address_string = ossia::net::address_string_from_node(*node);

    if (absolute_path != address_string) return false;

    if(node->findChild(x_name->s_name)){
        // pd_error(this, "a parameter with adress '%s' already exists.", x_name->s_name);
        x_node = nullptr;
        return false;
    }

    x_node = node->createChild(x_name->s_name);
    x_node->aboutToBeDeleted.connect<t_param, &t_param::isDeleted>(this);
    ossia::net::address_base* localAddress{};
    if (std::string(x_type->s_name) == "float") {
        localAddress = x_node->createAddress(ossia::val_type::FLOAT);
    } else if(std::string(x_type->s_name) == "symbol" || std::string(x_type->s_name) == "string"){
        localAddress = x_node->createAddress(ossia::val_type::STRING);
    } else if (std::string(x_type->s_name) == "int") {
        localAddress = x_node->createAddress(ossia::val_type::INT);
    } else if (std::string(x_type->s_name) == "vec2f") {
        localAddress = x_node->createAddress(ossia::val_type::VEC2F);
        x_type_size = 2;
    } else if (std::string(x_type->s_name) == "vec3f") {
        localAddress = x_node->createAddress(ossia::val_type::VEC3F);
        x_type_size = 3;
    } else if (std::string(x_type->s_name) == "vec4f") {
        localAddress = x_node->createAddress(ossia::val_type::VEC4F);
        x_type_size = 4;
    } else if (std::string(x_type->s_name) == "impulse") {
        localAddress = x_node->createAddress(ossia::val_type::IMPULSE);
        x_type_size = 0;
    } else if (std::string(x_type->s_name) == "bool") {
        localAddress = x_node->createAddress(ossia::val_type::BOOL);
    } else if (std::string(x_type->s_name) == "tuple") {
        localAddress = x_node->createAddress(ossia::val_type::TUPLE);
        x_type_size = 64;
    } else if (std::string(x_type->s_name) == "char") {
        localAddress = x_node->createAddress(ossia::val_type::CHAR);
    }

    localAddress->setDomain(ossia::make_domain(x_range[0],x_range[1]));
    // FIXME : we need case insensitive comparison here
    std::string bounding_mode = x_bounding_mode->s_name;
    if (bounding_mode == "FREE")
        localAddress->setBoundingMode(ossia::bounding_mode::FREE);
    else if (bounding_mode == "CLIP")
        localAddress->setBoundingMode(ossia::bounding_mode::CLIP);
    else if (bounding_mode == "WRAP")
        localAddress->setBoundingMode(ossia::bounding_mode::WRAP);
    else if (bounding_mode == "FOLD")
        localAddress->setBoundingMode(ossia::bounding_mode::FOLD);
    else if (bounding_mode == "LOW")
        localAddress->setBoundingMode(ossia::bounding_mode::LOW);
    else if (bounding_mode == "HIGH")
        localAddress->setBoundingMode(ossia::bounding_mode::HIGH);

    std::string access_mode = x_access_mode->s_name;
    if(access_mode == "BI" || access_mode == "RW")
        localAddress->setAccessMode(ossia::access_mode::BI);
    else if(access_mode == "GET" || access_mode == "R")
        localAddress->setAccessMode(ossia::access_mode::GET);
    else if(access_mode == "SET" || access_mode == "W")
        localAddress->setAccessMode(ossia::access_mode::SET);

    localAddress->setRepetitionFilter(x_repetition_filter? ossia::repetition_filter::ON : ossia::repetition_filter::OFF);

    ossia::unit_t unit = ossia::parse_pretty_unit(x_unit->s_name);
    localAddress->setUnit(unit);

    ossia::net::set_description(*x_node, x_description->s_name);
    ossia::net::set_tags(*x_node, parse_tags_symbol(x_tags));

    localAddress->add_callback([=](const ossia::value& v){
        setValue(v);
    });

    clock_delay(x_clock,0);

    return true;
}

bool t_param :: unregister(){
    if (x_node) {
        if (x_node->getParent()) x_node->getParent()->removeChild(x_name->s_name);
        x_node = nullptr;
        for (auto remote : t_remote::quarantine()){
            obj_register<t_remote>(static_cast<t_remote*>(remote));
        }
    }
    obj_quarantining<t_param>(this);
    return true;
}

static void *parameter_new(t_symbol *name, int argc, t_atom *argv)
{
    t_param *x = (t_param *)eobj_new(parameter_class);

    // TODO SANITIZE : memory leak
    t_binbuf* d = binbuf_via_atoms(argc,argv);

    if(x && d)
    {
        x->x_range[0] = 0.;
        x->x_range[1] = 1.;

        x->x_setout = nullptr;
        x->x_dataout = outlet_new((t_object*)x,nullptr);
        x->x_dumpout = outlet_new((t_object*)x,gensym("dumpout"));
        x->x_node = nullptr;

        x->x_access_mode = gensym("RW");
        x->x_bounding_mode = gensym("FREE");
        x->x_unit = gensym("");
        x->x_type = gensym("tuple");
        x->x_type_size = 1;
        x->x_tags = gensym("");
        x->x_description = gensym("");

        x->x_clock = clock_new(x, (t_method)push_default_value);

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
            if (std::string(x->x_name->s_name) != "" && x->x_name->s_name[0] == '/') x->x_absolute = true;

        } else {
            pd_error(x,"You have to pass a name as the first argument");
            x->x_name=gensym("untitledParam");
        }

        ebox_attrprocess_viabinbuf(x, d);

        obj_register<t_param>(x);
    }

    return (x);
}

static void parameter_free(t_param *x)
{
    x->unregister();
    obj_dequarantining<t_param>(x);
    outlet_free(x->x_dataout);
    outlet_free(x->x_dumpout);
}

extern "C" void setup_ossia0x2eparam(void)
{
    t_eclass *c = eclass_new("ossia.param", (method)parameter_new, (method)parameter_free, (short)sizeof(t_param), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) t_obj_base::obj_push, "anything", A_GIMME, 0);
        eclass_addmethod(c, (method) t_obj_base::obj_bang, "bang",     A_NULL, 0);
        eclass_addmethod(c, (method) obj_dump<t_param>,    "dump",       A_NULL, 0);

        CLASS_ATTR_SYMBOL     (c, "type",            0, t_param, x_type);
        CLASS_ATTR_SYMBOL     (c, "unit",            0, t_param, x_unit);
        CLASS_ATTR_SYMBOL     (c, "bounding_mode",   0, t_param, x_bounding_mode);
        CLASS_ATTR_SYMBOL     (c, "access_mode",     0, t_param, x_access_mode);

        CLASS_ATTR_ATOM_ARRAY (c, "default",         0, t_param, x_default, 64);
        CLASS_ATTR_FLOAT_ARRAY(c, "range",           0, t_param, x_range, 2);
        CLASS_ATTR_FLOAT      (c, "min",             0, t_param, x_range);
        CLASS_ATTR_FLOAT      (c, "repetition_filter", 0, t_param, x_repetition_filter);
        // CLASS_ATTR_FLOAT(c, "max", 0, t_parameter, range+1);
        eclass_new_attr_typed(c,"max", "float", 1, 0, 0, calcoffset(t_param,x_range)+sizeof(float));

        CLASS_ATTR_DEFAULT(c, "type", 0, "float");

        // eclass_register(CLASS_OBJ, c); // disable property dialog since it's buggy

    }

    parameter_class = c;
}

} } // namespace
