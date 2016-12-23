// A starter for Pd objects
#include "view.hpp"
#include "remote.hpp"

namespace ossia { namespace pd {

static t_eclass *view_class;

static void view_register(t_view *x)
{
    t_device* device = nullptr;

    int l;
    if (!(device = (t_device*) find_parent(&x->x_obj,osym_device, 0, &l)))
    {
        x->x_node = nullptr;
        return;
    }
    // look for an [ossia.view] instance in the parent patchers
    t_view* view = find_parent_alive<t_view>(&x->x_obj,osym_view, 1, &l);
    if (view)  {
        x->register_node(view->x_node);
        std::cout << "found a parent view" << std::endl;
        return;
    }
    std::cout << "found a parent device" << std::endl;
    x->register_node(device->x_node);
}

static void view_loadbang(t_view *x){
    std::cout << "[ossia.view] loadbang" << std::endl;
    view_register(x);
}

static void view_dump(t_view *x)
{
    t_atom a;
    std::string fullpath = get_absolute_path(x->x_node);
    SETSYMBOL(&a,gensym(fullpath.c_str()));
    outlet_anything(x->x_dumpout,gensym("fullpath"), 1, &a);
}

bool t_view :: register_node(ossia::net::node_base*  node){
    std::cout << "[ossia.view] : register view : " << x_name->s_name << std::endl;

    x_node = nullptr;

    if (node){
        x_node = node->findChild(x_name->s_name);
        if (x_node) {
            x_node->aboutToBeDeleted.connect<t_view, &t_view::isDeleted>(this);
            dequarantining();
        } else {
            quarantining();
            return false;
        }
    }

    std::vector<obj_hierachy> remotes = find_child(x_obj.o_canvas->gl_list, osym_remote, 0);
    std::sort(remotes.begin(), remotes.end());
    for (auto v : remotes){
        t_remote* param = (t_remote*) v.x;
        param->register_node(x_node);
    }

    std::vector<obj_hierachy> views = find_child(x_obj.o_canvas->gl_list, osym_view, 0);
    std::sort(views.begin(), views.end());
    for (auto v : views){
        t_view* view = (t_view*) v.x;
        view->register_node(x_node);
    }
    return true;
}

bool t_view :: unregister(){
    if(!x_node) return true; // not registered

    // when removing a view, we should re-register all its children to parent node
    std::vector<obj_hierachy> remotes = find_child(x_obj.o_canvas->gl_list, osym_remote, 0);
    std::sort(remotes.begin(), remotes.end());
    for (auto v : remotes){
        t_remote* remote = (t_remote*) v.x;
        if (!remote->x_node || remote->x_node->getParent() == x_node) remote->register_node(x_node->getParent());
    }

    std::vector<obj_hierachy> views = find_child(x_obj.o_canvas->gl_list, osym_view, 0);
    std::sort(views.begin(), views.end());
    for (auto v : views){
        t_view* view = (t_view*) v.x;
        if (view != this && (!view->x_node || view->x_node->getParent() == x_node)) view->register_node(x_node->getParent());
    }

    x_node->getParent()->removeChild(x_name->s_name);
    x_node = nullptr;

    return true;
}

static void *view_new(t_symbol *name, int argc, t_atom *argv)
{
    t_view *x = (t_view *)eobj_new(view_class);

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
    }

    return (x);
}

static void view_free(t_view *x)
{
    x->x_dead = true;
    x->unregister();
}

extern "C" void setup_ossia0x2eview(void)
{
    t_eclass *c = eclass_new("ossia.view", (method)view_new, (method)view_free, (short)sizeof(t_view), CLASS_DEFAULT, A_GIMME, 0);

    if(c)
    {
        eclass_addmethod(c, (method) view_loadbang,   "loadbang",   A_NULL, 0);
        eclass_addmethod(c, (method) view_dump,       "dump",       A_NULL, 0);
    }

    view_class = c;
}
} }
