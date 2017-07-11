// A starter for Pd objects
#include "model.hpp"
#include "parameter.hpp"
#include "view.hpp"
#include "remote.hpp"
#include <ossia/network/base/node_attributes.hpp>

namespace ossia { namespace pd {

static t_eclass *model_class;
static void model_free(t_model *x);

bool t_model :: register_node(ossia::net::node_base*  node){
    bool res = do_registration(node);
    if (res){
        obj_dequarantining<t_model>(this);
        std::vector<obj_hierachy> obj = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.model");
        for (auto v : obj){
            if(v.classname == "ossia.model"){
                t_model* model = (t_model*) v.x;
                if (model == this) {
                    // not registering itself
                    continue;
                }
                model->register_node(x_node);
            } else if(v.classname == "ossia.param"){
                t_param* param = (t_param*) v.x;
                param->register_node(x_node);
            }
        }

        for (auto view : t_view::quarantine()){
            obj_register<t_view>(static_cast<t_view*>(view));
        }

        // then try to register qurantinized remote
        for (auto remote : t_remote::quarantine()){
            obj_register<t_remote>(static_cast<t_remote*>(remote));
        }

    } else obj_quarantining<t_model>(this);

    return res;
}

bool t_model :: do_registration(ossia::net::node_base*  node){

    if (x_node && x_node->get_parent() == node ) return true; // already register to this node;
    unregister(); // we should unregister here because we may have add a node between the registered node and the parameter
    if (!node) return false;

    std::string name(x_name->s_name);

    if (node->find_child(name)) { // we have to check if a node with the same name already exists to avoid auto-incrementing name
        std::vector<obj_hierachy> obj = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.model");
        for (auto v : obj){
            if(v.classname == "ossia.param"){
                t_param* param = (t_param*) v.x;
                if (std::string(param->x_name->s_name) == name) {
                    param->unregister(); // if we already have a t_param node of that name, unregister it
                    // we will register it again after node creation
                    continue;
                }
            }
        }
    }

    x_node = &ossia::net::create_node(*node, name);
    x_node->about_to_be_deleted.connect<t_model, &t_model::isDeleted>(this);

    ossia::net::set_description(*x_node, std::string(x_description->s_name));
    ossia::net::set_tags(*x_node, parse_tags_symbol(x_tags));

    return true;
}

bool t_model :: unregister(){

    clock_unset(x_regclock);
    if(!x_node) return true; // not registered

    if (x_node && x_node->get_parent())
        x_node->get_parent()->remove_child(*x_node); // this calls isDeleted() on each registered child and put them into quarantine
    x_node = nullptr;

    obj_quarantining<t_model>(this);

    register_quarantinized();

    return true;
}

static void *model_new(t_symbol *name, int argc, t_atom *argv)
{
    t_model *x = (t_model *)eobj_new(model_class);

    t_binbuf* d = binbuf_via_atoms(argc,argv);
    if(x && d)
    {
        x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
        x->x_regclock = clock_new(x, (t_method)obj_register<t_model>);

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
            if (std::string(x->x_name->s_name) != "" && x->x_name->s_name[0] == '/') x->x_absolute = true;
        } else {
            x->x_name = gensym("untitledModel");
            pd_error(x,"You have to pass a name as the first argument");
        }

        x->x_description = gensym("");
        x->x_tags = gensym("");

        ebox_attrprocess_viabinbuf(x, d);

        // we need to delay registration because object may use patcher hierarchy to check address validity
        // and object will be added to patcher's objects list (aka canvas g_list) after model_new() returns.
        // 0 ms delay means that it will be perform on next clock tick
        clock_delay(x->x_regclock, 0);
    }

    t_gobj* list = x->x_obj.o_canvas->gl_list;
    while (list){
        std::string current = list->g_pd->c_name->s_name;
        if ( current == "ossia.model" ) {
            if ( x != (t_model*) &list->g_pd ) {
                pd_error(&list->g_pd,"Only one [ossia.model] intance per patcher is allowed.");
                model_free(x);
                x = nullptr;
                break;
            }
        }
        list=list->g_next;
    }

    return (x);
}

static void model_free(t_model *x)
{
    x->x_dead = true;
    x->unregister();
    obj_dequarantining<t_model>(x);
    clock_free(x->x_regclock);
}

static void model_bind(t_model* x, t_symbol* address){
    x->x_name = address;
    if (std::string(x->x_name->s_name) != "" && x->x_name->s_name[0] == '/') x->x_absolute = true;
    obj_register(x);
}

extern "C" void setup_ossia0x2emodel(void)
{
    t_eclass *c = eclass_new("ossia.model", (method)model_new, (method)model_free, (short)sizeof(t_model), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) obj_dump<t_model>,   "dump",  A_NULL,   0);
        eclass_addmethod(c, (method) model_bind,          "bind",  A_SYMBOL, 0);

        CLASS_ATTR_SYMBOL(c, "description", 0, t_model, x_description);
        CLASS_ATTR_SYMBOL(c, "tags",        0, t_model, x_tags);

        // eclass_register(CLASS_OBJ,c); // disable property dialog since it's buggy
        model_class = c;
    }
}

} } // namespace
