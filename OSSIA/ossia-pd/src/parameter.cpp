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

bool t_param :: do_registration(ossia::net::node_base* node){

    if (x_node && x_node->getParent() == node ) return true; // already register to this node;

    unregister(); // we should unregister here because we may have add a node between the registered node and the parameter

    if(node){
        x_node = node->findChild(x_name->s_name);
        if(x_node){
            // pd_error(this, "a parameter with adress '%s' already exists.", x_name->s_name);
            x_node = nullptr;
            return false;
        }

        x_node = node->createChild(x_name->s_name);
        x_node->aboutToBeDeleted.connect<t_param, &t_param::isDeleted>(this);
        ossia::net::address_base* localAddress{};
        if(x_type == gensym("symbol")){
            localAddress = x_node->createAddress(ossia::val_type::STRING);
        } else {
            localAddress = x_node->createAddress(ossia::val_type::FLOAT);
            localAddress->setDomain(ossia::make_domain(x_range[0],x_range[1]));
            // FIXME : we need case insensitive comparison here
            if (x_bounding_mode == gensym("FREE"))
              localAddress->setBoundingMode(ossia::bounding_mode::FREE);
            else if (x_bounding_mode == gensym("CLIP"))
              localAddress->setBoundingMode(ossia::bounding_mode::CLIP);
            else if (x_bounding_mode == gensym("WRAP"))
              localAddress->setBoundingMode(ossia::bounding_mode::WRAP);
            else if (x_bounding_mode == gensym("FOLD"))
              localAddress->setBoundingMode(ossia::bounding_mode::FOLD);
            else if (x_bounding_mode == gensym("LOW"))
              localAddress->setBoundingMode(ossia::bounding_mode::LOW);
            else if (x_bounding_mode == gensym("HIGH"))
              localAddress->setBoundingMode(ossia::bounding_mode::HIGH);

            if(x_access_mode == gensym("BI") || x_access_mode == gensym("RW"))
              localAddress->setAccessMode(ossia::access_mode::BI);
            else if(x_access_mode == gensym("GET") || x_access_mode == gensym("R"))
                localAddress->setAccessMode(ossia::access_mode::GET);
            else if(x_access_mode == gensym("SET") || x_access_mode == gensym("W"))
                localAddress->setAccessMode(ossia::access_mode::SET);

            localAddress->setRepetitionFilter(x_repetition_filter? ossia::repetition_filter::ON : ossia::repetition_filter::OFF);

            ossia::unit_t unit = ossia::parse_pretty_unit(x_unit->s_name);
            localAddress->setUnit(unit);

            ossia::net::set_description(*x_node, x_description->s_name);
            ossia::net::set_tags(*x_node, parse_tags_symbol(x_tags));

        }
        localAddress->add_callback([=](const ossia::value& v){
            setValue(v);
        });
        if (x_default.a_type != A_NULL){
            obj_setList<t_param>(this,gensym("set"),1,&x_default);
        }
    } else {
        return false;
    }

    return true;
}

bool t_param :: unregister(){
    if (x_node) {
        x_node->getParent()->removeChild(x_name->s_name);
        x_node = nullptr;
        obj_quarantining<t_param>(this);
    }
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

        x->x_setout  = outlet_new((t_object*)x,nullptr);
        x->x_dataout = outlet_new((t_object*)x,nullptr);
        x->x_dumpout = outlet_new((t_object*)x,gensym("dumpout"));
        x->x_node = nullptr;

        x->x_access_mode = gensym("RW");
        x->x_bounding_mode = gensym("FREE");
        x->x_unit = gensym("");
        x->x_type = gensym("float");
        x->x_tags = gensym("");
        x->x_description = gensym("");

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
            if (x->x_name != osym_empty && x->x_name->s_name[0] == '/') x->x_absolute = true;

        } else {
            pd_error(x,"You have to pass a name as the first argument");
            x->x_name=gensym("untitledParam");
        }

        ebox_attrprocess_viabinbuf(x, d);

        // if we only pass a default value without setting parameter type,
        // the type is deduced from the default value (for now in Pd only symbol and float)
        if(x->x_default.a_type == A_SYMBOL) x->x_type = gensym("symbol");
        else x->x_type = gensym("float");

        obj_register<t_param>(x);
    }

    return (x);
}

static void parameter_free(t_param *x)
{
    x->unregister();
    obj_dequarantining<t_param>(x);
    outlet_free(x->x_dataout);
    outlet_free(x->x_setout);
    outlet_free(x->x_dumpout);
}

extern "C" void setup_ossia0x2eparam(void)
{
    t_eclass *c = eclass_new("ossia.param", (method)parameter_new, (method)parameter_free, (short)sizeof(t_param), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) obj_setFloat<t_param>,     "float",      A_FLOAT, 0);
        eclass_addmethod(c, (method) obj_setSymbol<t_param>,     "symbol",     A_SYMBOL, 0);
        eclass_addmethod(c, (method) obj_setList<t_param>,     "list",       A_GIMME, 0);
        eclass_addmethod(c, (method) obj_bang<t_param>,    "bang",       A_NULL, 0);
        eclass_addmethod(c, (method) obj_dump<t_param>,    "dump",       A_NULL, 0);

        CLASS_ATTR_SYMBOL     (c, "type",            0, t_param, x_type);
        CLASS_ATTR_SYMBOL     (c, "unit",            0, t_param, x_unit);
        CLASS_ATTR_SYMBOL     (c, "bounding_mode",   0, t_param, x_bounding_mode);
        CLASS_ATTR_SYMBOL     (c, "access_mode",     0, t_param, x_access_mode);

        CLASS_ATTR_ATOM       (c, "default",         0, t_param, x_default);
        CLASS_ATTR_FLOAT_ARRAY(c, "range",           0, t_param, x_range, 2);
        CLASS_ATTR_FLOAT      (c, "min",             0, t_param, x_range);
        CLASS_ATTR_FLOAT      (c, "repetition_filter", 0, t_param, x_repetition_filter);
        // CLASS_ATTR_FLOAT(c, "max", 0, t_parameter, range+1);
        eclass_new_attr_typed(c,"max", "float", 1, 0, 0, calcoffset(t_param,x_range)+sizeof(float));

        CLASS_ATTR_DEFAULT(c, "type", 0, "float");

        // eclass_register(CLASS_OBJ, c);

    }

    parameter_class = c;
}

} } // namespace
