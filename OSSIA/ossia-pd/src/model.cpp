// A starter for Pd objects
#include "model.hpp"
#include "parameter.hpp"
#include "view.hpp"
#include "remote.hpp"

namespace ossia { namespace pd {

static t_eclass *model_class;

static void model_register(t_model *x)
{
    t_device* device = nullptr;

    int l;
    if (!(device = (t_device*) find_parent(&x->x_obj,osym_device, 0, &l)))
    {
        x->x_node = nullptr;
        return;
    }
    // look for an [ossia.model] instance in the parent patcher
    t_model* model = find_parent_alive<t_model>(&x->x_obj,osym_model, 1, &l);
    if (model)  {
        x->register_node(model->x_node);
        return;
    }
    x->register_node(device->x_node);
}

/*
static void model_loadbang(t_model *x){
    // obj_register<t_model>(x);
    model_register(x);
}
*/

bool t_model :: register_node(ossia::net::node_base*  node){
    if (!node) return false;

    if (x_node && x_node->getParent() == node ) return true; // already register to this node;
    unregister(); // we should unregister here because we may have add a node between the registered node and the parameter

    x_node = node->findChild(x_name->s_name);
    if (!x_node) x_node = node->createChild(x_name->s_name);

    x_node->aboutToBeDeleted.connect<ossia_obj_base, &ossia_obj_base::isDeleted>(this);

    std::vector<obj_hierachy> params = find_child(x_obj.o_canvas->gl_list, osym_param, 0);
    // std::sort(params.begin(), params.end());
    for (auto v : params){
        t_param* param = (t_param*) v.x;
        param->register_node(x_node);
    }

    // then try to register
    for (auto param : t_param::quarantine()){
        obj_register<t_param>(static_cast<t_param*>(param));
    }

    // FIXME nested model is not registered properly
    std::vector<obj_hierachy> models = find_child(x_obj.o_canvas->gl_list, osym_model, 1);
    // std::sort(models.begin(), models.end());
    for (auto v : models){
        t_model* model = (t_model*) v.x;
        model->register_node(x_node);
    }

    for (auto view : t_view::quarantine()){
        obj_register<t_view>(static_cast<t_view*>(view));
    }

    return true;
}

bool t_model :: unregister(){

    // TODO refactor
    if(!x_node) return true; // not registered

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

    x_node->getParent()->removeChild(x_name->s_name);
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
            if (x->x_name != osym_empty && x->x_name->s_name[0] == '/') x->x_absolute = true;
        } else {
            x->x_name = gensym("untitledModel");
            pd_error(x,"You have to pass a name as the first argument");
        }

        obj_register<t_model>(x);
    }

    return (x);
}

static void model_free(t_model *x)
{
    x->x_dead = true;
    x->unregister();
}

extern "C" void setup_ossia0x2emodel(void)
{
    t_eclass *c = eclass_new("ossia.model", (method)model_new, (method)model_free, (short)sizeof(t_model), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) obj_dump<t_model>,   "dump",       A_NULL, 0);
    }

    model_class = c;
}

} } // namespace
