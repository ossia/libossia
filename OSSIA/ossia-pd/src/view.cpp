// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// A starter for Pd objects
#include "view.hpp"
#include "remote.hpp"
#include "utils.hpp"
#include "ossia-pd.hpp"

namespace ossia
{
namespace pd
{

static t_eclass* view_class;
static void view_free(t_view* x);

//****************//
// Member methods //
//****************//
bool t_view::register_node(ossia::net::node_base* node)
{
  bool res = do_registration(node);
  if (res)
  {
    obj_dequarantining<t_view>(this);
    std::vector<t_obj_base*> viewnode
        = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.view");
    for (auto v : viewnode)
    {
      if (v->x_otype == Type::view)
      {
        t_view* view = (t_view*)v;
        if (view == this)
        {
          // not registering itself
          continue;
        }
        view->register_node(x_node);
      }
      else if (v->x_otype == Type::remote)
      {
        t_remote* remote = (t_remote*)v;
        remote->register_node(x_node);
      }
    }
  }
  else
    obj_quarantining<t_view>(this);

  return res;
}

bool t_view::do_registration(ossia::net::node_base* node)
{
  if (x_node && x_node->get_parent() == node)
    return true; // already register to this node;
  unregister();  // we should unregister here because we may have add a node
                 // between the registered node and the remote

  if (node)
  {
    if (x_absolute == AddrType::relative)
    {
      x_node = node->find_child(x_name->s_name);
    }
    else if(x_absolute == AddrType::absolute)
    {
      x_node = ossia::net::find_node(
            node->get_device().get_root_node(), x_name->s_name);
    } else {
      x_node = ossia::pd::find_global_node(x_name->s_name);
    }


    if (x_node)
    {
      x_node->about_to_be_deleted.connect<t_view, &t_view::is_deleted>(this);
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  return true;
}

static void register_children(t_view* x)
{
  std::vector<t_obj_base*> viewnode
      = find_child_to_register(x, x->x_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnode)
  {
    if (v->x_otype == Type::view)
    {
      t_view* view = (t_view*)v;
      if (view == x)
        continue;
      obj_register<t_view>(view);
    }
    else if (v->x_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      obj_register<t_remote>(remote);
    }
  }
}

bool t_view::unregister()
{
  if (!x_node)
    return true; // not registered

  x_node->about_to_be_deleted.disconnect<t_view, &t_view::is_deleted>(this);

  std::vector<t_obj_base*> viewnode
      = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.view");
  for (auto v : viewnode)
  {
    if (v->x_otype == Type::view)
    {
      t_view* view = (t_view*)v;
      if (view == this)
        continue;
      view->unregister();
    }
    else if (v->x_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      remote->unregister();
    }
  }
  x_node = nullptr;
  obj_quarantining<t_view>(this);

  register_children(this);

  return true;
}

static void view_click(
    t_view* x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift,
    t_floatarg ctrl, t_floatarg alt)
{

  using namespace std::chrono;
  milliseconds ms
      = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  milliseconds diff = (ms - x->x_last_click);
  if (diff.count() < 200)
  { // 200 ms double click
    x->x_last_click = milliseconds(0);

    int l;
    t_device* device
        = (t_device*)find_parent(&x->x_obj, "ossia.device", 0, &l);
    /*
    if (!device || !x->x_node || obj_isQuarantined<t_remote>(x)){
      pd_error(x, "sorry no device found, or not connected or quarantined...");
      return;
    }
    */

    t_canvas* root = x->x_obj.o_canvas;
    while (root->gl_owner)
      root = root->gl_owner;
    if (!find_and_display_friend(x, root))
      pd_error(x, "sorry I can't find a connected friend :-(");
  }
  else
  {
    x->x_last_click = ms;
  }
}

static void* view_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_view* x = (t_view*)eobj_new(ossia_pd.view);

  if (x)
  {
    ossia_pd.views.push_back(x);

    x->x_otype = Type::view;
    x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
    x->x_clock = clock_new(x, (t_method)obj_tick);
    x->x_regclock = clock_new(x, (t_method)obj_register<t_view>);

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->x_name = atom_getsymbol(argv);
      x->x_absolute = ossia::pd::get_address_type(x->x_name->s_name);

      // we need to delay registration because object may use patcher hierarchy
      // to check address validity
      // and object will be added to patcher's objects list (aka canvas g_list)
      // after model_new() returns.
      // 0 ms delay means that it will be perform on next clock tick
      clock_delay(x->x_regclock, 0);
    }
    else
    {
      x->x_name = gensym("untitledModel");
      pd_error(x, "You have to pass a name as the first argument");
    }

    if (find_peer(x))
    {
      error(
          "Only one [ø.model]/[ø.view] intance per patcher is allowed.");
      view_free(x);
      x = nullptr;
    }
  }


  return x;
}

static void view_free(t_view* x)
{
  x->x_dead = true;
  x->unregister();
  obj_dequarantining<t_view>(x);
  ossia_pd::instance().views.remove_all(x);
  clock_free(x->x_regclock);
  clock_free(x->x_clock);
}

static void view_bind(t_view* x, t_symbol* address)
{
  x->x_name = address;
  x->x_absolute = ossia::pd::get_address_type(x->x_name->s_name);
  x->unregister();
  obj_register(x);
}

extern "C" void setup_ossia0x2eview(void)
{
  t_eclass* c = eclass_new(
      "ossia.view", (method)view_new, (method)view_free, (short)sizeof(t_view),
      CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)view_new,gensym("ø.view"), A_GIMME, 0);

    eclass_addmethod(c, (method)obj_dump<t_view>, "dump", A_NULL, 0);
    eclass_addmethod(c, (method)view_click, "click", A_NULL, 0);
    eclass_addmethod(c, (method)view_bind, "bind", A_SYMBOL, 0);
    eclass_addmethod(c, (method)obj_namespace, "namespace", A_NULL, 0);
  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.view = c;
}
} // pd namespace
} // ossia namespace
