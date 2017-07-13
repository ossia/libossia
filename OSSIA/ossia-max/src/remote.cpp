// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "remote.hpp"
#include "device.hpp"
#include "parameter.hpp"

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_remote class methods

extern "C" void ossia_remote_setup(void)
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.remote class
  ossia_library.ossia_remote_class = class_new(
      "ossia.remote", (method)ossia_remote_new, (method)ossia_remote_free,
      (short)sizeof(t_remote), 0L, A_GIMME, 0);

  if (ossia_library.ossia_remote_class)
  {
    class_addmethod(
        ossia_library.ossia_remote_class, (method)t_object_base::push,
        "anything", A_GIMME, 0);
    class_addmethod(
        ossia_library.ossia_remote_class, (method)t_object_base::bang, "bang",
        A_NOTHING, 0);
    class_addmethod(
        ossia_library.ossia_remote_class, (method)object_dump<t_remote>,
        "dump", A_NOTHING, 0);
    //        class_addmethod(ossia_library.ossia_remote_class,
    //        (method)ossia_remote_click,             "click",
    //        A_NOTHING,     0);

    class_addmethod(
        ossia_library.ossia_remote_class, (method)ossia_remote_assist,
        "assist", A_CANT, 0);
    class_addmethod(
        ossia_library.ossia_remote_class, (method)ossia_remote_in_bang, "bang",
        0);
    class_addmethod(
        ossia_library.ossia_remote_class, (method)ossia_remote_in_int, "int",
        A_LONG, 0);
    class_addmethod(
        ossia_library.ossia_remote_class, (method)ossia_remote_in_float,
        "float", A_FLOAT, 0);
    class_addmethod(
        ossia_library.ossia_remote_class, (method)ossia_remote_in_symbol,
        "symbol", A_SYM, 0);
    class_addmethod(
        ossia_library.ossia_remote_class, (method)ossia_remote_in_anything,
        "anything", A_GIMME, 0);
  }

  class_register(CLASS_BOX, ossia_library.ossia_remote_class);
}

extern "C" void* ossia_remote_new(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  t_remote* x = (t_remote*)object_alloc(ossia_library.ossia_remote_class);

  if (x)
  {
    // make outlets
    x->m_dump_out
        = outlet_new(x, NULL); // anything outlet to dump remote state
    x->m_set_out
        = outlet_new(x, NULL); // anything outlet to output data for ui
    x->m_data_out = outlet_new(x, NULL); // anything outlet to output data

    x->m_callbackit = boost::none;

    //        x->m_clock = clock_new(x, (method)t_object_base::tick);
    x->m_regclock = clock_new(x, (method)t_object_base::bang);

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_absolute = std::string(x->m_name->s_name) != ""
                        && x->m_name->s_name[0] == '/';
      }
    }

    if (x->m_name == _sym_nothing)
    {
      object_error((t_object*)x, "needs a name as first argument");
      x->m_name = gensym("untitledRemote");
      return x;
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    object_register<t_remote>(x);
  }

  return (x);
}

extern "C" void ossia_remote_free(t_remote* x)
{
  x->m_dead = true;
  x->unregister();
  object_dequarantining<t_remote>(x);
  // TODO : free outlets
}

extern "C" void
ossia_remote_assist(t_remote* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "I am inlet %ld", a);
  }
  else
  {
    sprintf(s, "I am outlet %ld", a);
  }
}

/*
extern "C"
void ossia_remote_click(t_remote *x, t_floatarg xpos, t_floatarg ypos,
t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{
    using namespace std::chrono;

    milliseconds ms = duration_cast< milliseconds >(
system_clock::now().time_since_epoch() );

    milliseconds diff = (ms - x->m_last_click);

    if (diff.count() < 200)
    {
        x->m_last_click = milliseconds(0);

        int l;
        t_device *device = (t_device*) find_parent(&x->m_obj, "ossia.device",
0, &l);
*/
/*
 if (!device || !x->m_node || obj_isQuarantined<t_remote>(x)){
 pd_error(x, "sorry no device found, or not connected or quarantined...");
 return;
 }
 */
/*
        t_canvas *root = x->m_obj.o_canvas;
        while (root->gl_owner)
            root = root->gl_owner;

        if (!find_and_display_friend(x, root))
            pd_error(x,"sorry I can't find a connected friend :-(");
    }
    else
        x->m_last_click = ms;
}
*/

template <typename T>
void ossia_remote_in(t_remote* x, T f)
{
  if (x && x->m_node)
  {
    if (auto addr = x->m_node->get_address())
    {
      addr->push_value(f);
    }
  }
  else
  {
    object_error(
        (t_object*)x, "[ossia.remote %s] is not registered to any parameter",
        x->m_name->s_name);
  }
}

extern "C" void ossia_remote_in_float(t_remote* x, double f)
{
  ossia_remote_in(x, f);
}

extern "C" void ossia_remote_in_int(t_remote* x, long int f)
{
  ossia_remote_in(x, (int32_t)f);
}

extern "C" void ossia_remote_in_bang(t_remote* x)
{
  ossia_remote_in(x, ossia::impulse{});
}

extern "C" void ossia_remote_in_symbol(t_remote* x, t_symbol* f)
{
  ossia_remote_in(x, std::string(f->s_name));
}

extern "C" void ossia_remote_in_char(t_remote* x, char f)
{
  ossia_remote_in(x, f);
}

extern "C" void
ossia_remote_in_anything(t_remote* x, t_symbol* s, long argc, t_atom* argv)
{
  if (argc == 0)
  {
    ossia_remote_in(x, std::string(s->s_name));
  }
  else
  {
    /* todo */
  }
}

namespace ossia
{
namespace max
{

bool t_remote::register_node(ossia::net::node_base* node)
{
  bool res = do_registration(node);

  if (res)
  {
    object_dequarantining<t_remote>(this);
  }
  else
    object_quarantining<t_remote>(this);

  return res;
}

bool t_remote::do_registration(ossia::net::node_base* node)
{
  if (m_node && m_node->get_parent() == node)
  {
    return true; // already registered to this node;
  }

  if (node)
  {
    if (m_absolute)
    {
      m_node = ossia::net::find_node(*node, m_name->s_name);
    }
    else
    {
      std::string absolute_path = object_path_absolute<t_remote>(this);
      std::string address_string = ossia::net::address_string_from_node(*node);

      if (absolute_path != address_string)
        return false;

      m_node = ossia::net::find_node(*node, m_name->s_name);
    }

    if (m_node)
    {
      m_callbackit = m_node->get_address()->add_callback(
          [=](const ossia::value& v) { apply_value_visitor(v); });
      m_node->about_to_be_deleted.connect<t_remote, &t_remote::is_deleted>(
          this);

      clock_delay(m_regclock, 0);

      return true;
    }
  }

  return false;
}

bool t_remote::unregister()
{
  if (m_callbackit != boost::none)
  {
    if (m_node && m_node->get_address())
      m_node->get_address()->remove_callback(*m_callbackit);

    m_callbackit = boost::none;
  }

  object_quarantining<t_remote>(this);

  m_node = nullptr;
  return true;
}

} // max namespace
} // ossia namespace
