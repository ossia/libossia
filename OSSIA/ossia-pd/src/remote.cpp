// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "remote.hpp"
#include "device.hpp"
#include "parameter.hpp"
#include "utils.hpp"

namespace ossia
{
namespace pd
{

static void remote_free(t_remote* x);

bool t_remote::register_node(ossia::net::node_base* node)
{
  bool res = do_registration(node);
  if (res)
  {
    obj_dequarantining<t_remote>(this);
  }
  else
    obj_quarantining<t_remote>(this);

  return res;
}

bool t_remote::do_registration(ossia::net::node_base* node)
{

  if (x_node && x_node->get_parent() == node)
  {
    return true; // already registered to this node;
  }

  if (node)
  {
    if (x_absolute)
    {
      x_node = ossia::net::find_node(
            node->get_device().get_root_node(), x_name->s_name);
    }
    else
    {
      std::string absolute_path = get_absolute_path<t_remote>(this);
      std::string address_string = ossia::net::address_string_from_node(*node);
      if (absolute_path != address_string)
        return false;
      x_node = ossia::net::find_node(*node, x_name->s_name);
      if (x_node && !x_node->get_address()){
        fmt::MemoryWriter path;
        path << x_name->s_name << "/" << x_name->s_name;
        x_node = ossia::net::find_node(*node, path.str());
      }
    }
    if (x_node && x_node->get_address())
    {
      x_callbackit = x_node->get_address()->add_callback(
          [=](const ossia::value& v) { setValue(v); });

      x_node->about_to_be_deleted.connect<t_remote, &t_remote::is_deleted>(
          this);

      clock_delay(x_regclock, 0);

      return true;
    }
  }
  return false;
}

bool t_remote::unregister()
{
  if (x_node)
  {
    x_node->about_to_be_deleted.disconnect<t_remote, &t_remote::is_deleted>(
        this);
  }

  if (x_callbackit != boost::none)
  {
    if (x_node && x_node->get_address())
      x_node->get_address()->remove_callback(*x_callbackit);

    x_callbackit = boost::none;
  }
  obj_quarantining<t_remote>(this);

  x_node = nullptr;
  return true;
}

static void remote_float(t_remote* x, t_float val)
{
  if (x->x_node && x->x_node->get_address())
  {
    x->x_node->get_address()->push_value(float(val));
  }
  else
  {
    pd_error(
        x, "[ossia.remote %s] is not registered to any parameter",
        x->x_name->s_name);
  }
}

static void remote_click(
    t_remote* x, t_floatarg xpos, t_floatarg ypos, t_floatarg shift,
    t_floatarg ctrl, t_floatarg alt)
{

  using namespace std::chrono;
  milliseconds ms
      = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
  milliseconds diff = (ms - x->x_last_click);
  if (diff.count() < 200)
  {
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

static void* remote_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_remote* x = (t_remote*)eobj_new(ossia_pd.remote);

  if (x)
  {
    ossia_pd.remotes.push_back(x);

    x->x_otype = Type::remote;
    x->x_setout = outlet_new((t_object*)x, nullptr);
    x->x_dataout = outlet_new((t_object*)x, nullptr);
    x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
    x->x_callbackit = boost::none;

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      x->x_name = atom_getsymbol(argv);
      if (std::string(x->x_name->s_name) != "" && x->x_name->s_name[0] == '/')
        x->x_absolute = true;
    }
    else
    {
      error("You have to pass a name as the first argument");
      x->x_name = gensym("untitledRemote");
    }

    x->x_clock = clock_new(x, (t_method)obj_tick);
    x->x_regclock = clock_new(x, (t_method)t_obj_base::obj_bang);

    obj_register<t_remote>(x);
  }

  return (x);
}

static void remote_free(t_remote* x)
{
  x->x_dead = true;
  x->unregister();
  obj_dequarantining<t_remote>(x);
  ossia_pd::instance().remotes.remove_all(x);
  outlet_free(x->x_setout);
  outlet_free(x->x_dataout);
  outlet_free(x->x_dumpout);
}

extern "C" void setup_ossia0x2eremote(void)
{
  t_eclass* c = eclass_new(
      "ossia.remote", (method)remote_new, (method)remote_free,
      (short)sizeof(t_remote), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)remote_new,gensym("ø.remote"), A_GIMME, 0);

    eclass_addmethod(c, (method)t_obj_base::obj_push, "anything", A_GIMME, 0);
    eclass_addmethod(c, (method)t_obj_base::obj_bang, "bang", A_NULL, 0);
    eclass_addmethod(c, (method)obj_dump<t_remote>, "dump", A_NULL, 0);
    eclass_addmethod(c, (method)remote_click, "click", A_NULL, 0);
  }

  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.remote = c;
}
} // pd namespace
} // ossia namespace
