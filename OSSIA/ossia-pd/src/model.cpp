// A starter for Pd objects
#include "model.hpp"
#include "parameter.hpp"
#include "view.hpp"
#include "remote.hpp"
#include <ossia/network/base/node_attributes.hpp>

namespace ossia { namespace pd {

static t_eclass *model_class;

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
/*
        // then try to register quarantinized parameter
        for (auto param : t_param::quarantine()){
            obj_register<t_param>(static_cast<t_param*>(param));
        }
*/
        // then try to register qurantinized remote
        for (auto remote : t_remote::quarantine()){
            obj_register<t_remote>(static_cast<t_remote*>(remote));
        }
/*
        std::vector<obj_hierachy> models = find_child(x_obj.o_canvas->gl_list, osym_model, 1);
        for (auto v : models){
            t_model* model = (t_model*) v.x;
            model->register_node(x_node);
        }
*/
        for (auto view : t_view::quarantine()){
            obj_register<t_view>(static_cast<t_view*>(view));
        }
    } else obj_quarantining<t_model>(this);

    return res;
}

bool t_model :: do_registration(ossia::net::node_base*  node){

    if (x_node && x_node->getParent() == node ) return true; // already register to this node;
    unregister(); // we should unregister here because we may have add a node between the registered node and the parameter
    if (!node) return false;

    if (node->findChild(x_name->s_name)) {
        bool abord=true;
        std::vector<obj_hierachy> obj = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.model");
        for (auto v : obj){
            if(v.classname == "ossia.param"){
                t_param* param = (t_param*) v.x;
                if (std::string(param->x_name->s_name) == std::string(x_name->s_name)) {
                    abord = false;
                    param->unregister(); // if we already have a t_param node of that name, unregister it
                    // we will register it again after node creation
                    continue;
                }
            } else if (v.classname == "ossia.model"){
                t_model* model = (t_model*) v.x;
                if (std::string(model->x_name->s_name) == std::string(x_name->s_name)) {
                    abord = false;
                    model->unregister(); // if we already have a t_param node of that name, unregister it
                    // we will register it again after node creation
                    continue;
                }
            }
        }

        if (abord){
            pd_error((t_object*) this, "node %s aldready exists", x_name->s_name);
            return false;
        }
    }

    x_node = node->createChild(x_name->s_name);
    x_node->aboutToBeDeleted.connect<t_model, &t_model::isDeleted>(this);

    ossia::net::set_description(*x_node, x_description->s_name);
    ossia::net::set_tags(*x_node, parse_tags_symbol(x_tags));

    return true;
}

bool t_model :: unregister(){

    // TODO refactor
    if(!x_node) return true; // not registered

    std::vector<obj_hierachy> node = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.model");
    for (auto v : node){
        if(v.classname == "ossia.model"){
            t_model* model = (t_model*) v.x;
            if (model == this) continue;
            model->unregister();
        } else if(v.classname == "ossia.param"){
            t_param* param = (t_param*) v.x;
            param->unregister();
        }
    }

    /*
    // when removing a model, we should re-register all its children to parent node
    std::vector<obj_hierachy> params = find_child(x_obj.o_canvas->gl_list, osym_param, 0);
    std::sort(params.begin(), params.end());
    for (auto v : params){
        t_param* param = (t_param*) v.x;
        if (!param->x_node || param->x_node->getParent() == x_node) param->register_node(x_node->getParent());
    }

    std::vector<obj_hierachy> models = find_child(x_obj.o_canvas->gl_list, osym_model, 0);
    std::sort(models.begin(), models.end());
    for (auto v : models){
        t_model* model = (t_model*) v.x;
        if (model != this && (!model->x_node || model->x_node->getParent() == x_node)) model->register_node(x_node->getParent());
    }
    */

    if (x_node && x_node->getParent()) x_node->getParent()->removeChild(x_name->s_name);
    x_node = nullptr;
    obj_quarantining<t_model>(this);

    return true;
}

static void *model_new(t_symbol *name, int argc, t_atom *argv)
{
    t_model *x = (t_model *)eobj_new(model_class);

    if(x)
    {
        x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

        if (argc != 0 && argv[0].a_type == A_SYMBOL) {
            x->x_name = atom_getsymbol(argv);
            if (std::string(x->x_name->s_name) != "" && x->x_name->s_name[0] == '/') x->x_absolute = true;
        } else {
            x->x_name = gensym("untitledModel");
            pd_error(x,"You have to pass a name as the first argument");
        }

        x->x_description = gensym("");
        x->x_tags = gensym("");

        obj_register<t_model>(x);
    }

    return (x);
}

static void model_free(t_model *x)
{
    x->x_dead = true;
    x->unregister();
    obj_dequarantining<t_model>(x);
}

extern "C" void setup_ossia0x2emodel(void)
{
    t_eclass *c = eclass_new("ossia.model", (method)model_new, (method)model_free, (short)sizeof(t_model), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) obj_dump<t_model>,   "dump",       A_NULL, 0);
    }

    CLASS_ATTR_SYMBOL(c, "description", 0, t_model, x_description);
    CLASS_ATTR_SYMBOL(c, "tags",        0, t_model, x_tags);

    model_class = c;
}

} } // namespace
