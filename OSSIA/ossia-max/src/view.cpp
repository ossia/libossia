// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "view.hpp"
#include "remote.hpp"
#include "utils.hpp"

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_remote class methods

extern "C" void ossia_view_setup(void)
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.view class
  ossia_library.ossia_view_class = class_new(
      "ossia.view", (method)ossia_view_new, (method)ossia_view_free,
      (short)sizeof(t_view), 0L, A_GIMME, 0);

  if (ossia_library.ossia_view_class)
  {
    class_addmethod(
        ossia_library.ossia_view_class, (method)object_dump<t_view>, "dump",
        A_NOTHING, 0);

    class_addmethod(
          ossia_library.ossia_view_class, (method)t_object_base::relative_namespace,
                  "namespace", A_NOTHING, 0);

    //        class_addmethod(ossia_library.ossia_view_class,
    //        (method)ossia_view_click,       "click",      A_NOTHING,   0);
  }

  class_register(CLASS_BOX, ossia_library.ossia_view_class);
  class_alias(ossia_library.ossia_view_class, gensym("ø.view"));
}

extern "C" void* ossia_view_new(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  t_view* x = (t_view*)object_alloc(ossia_library.ossia_view_class);

  if(find_peers(x))
  {
    error("you can put only one [ossia.view] per patcher");
    object_free(x);
    return nullptr;
  }

  if (x)
  {
    // make outlets
    x->m_dump_out = outlet_new(x, NULL); // anything outlet to dump view state

    //        x->m_clock = clock_new(x, (method)t_object_base::tick);
    x->m_regclock = clock_new(
        x, reinterpret_cast<method>(
               static_cast<bool (*)(t_view*)>(&max_object_register<t_view>)));

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    x->m_otype = Type::view;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_absolute = std::string(x->m_name->s_name) != ""
                        && x->m_name->s_name[0] == '/';
      }

      // we need to delay registration because object may use patcher hierarchy
      // to check address validity
      // and object will be added to patcher's objects list (aka canvas g_list)
      // after model_new() returns.
      // 0 ms delay means that it will be perform on next clock tick
      clock_delay(x->m_regclock, 0);
    }

    if (x->m_name == _sym_nothing)
    {
      object_error((t_object*)x, "needs a name as first argument");
      x->m_name = gensym("untitledView");
      return x;
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);
  }

  /* théo : what is this for ???

      t_gobj* list = x->m_obj.o_canvas->gl_list;
      while (list)
      {
          std::string current = list->g_pd->c_name->s_name;
          if ( current == "ossia.view" )
          {
              if ( x != (t_view*) &list->g_pd )
              {
                  pd_error(&list->g_pd, "Only one [ossia.view] intance per
     patcher is allowed.");
                  view_free(x);
                  x = nullptr;
                  break;
              }
          }

          list=list->g_next;
      }
  */
  return (x);
}

extern "C" void ossia_view_free(t_view* x)
{
  x->m_dead = true;
  x->unregister();
  object_dequarantining<t_view>(x);
  object_free(x->m_regclock);
  object_free(x->m_clock);
  if(x->m_dump_out) outlet_delete(x->m_dump_out);
}
/*
extern "C"
void ossia_view_click(t_view *x, t_floatarg xpos, t_floatarg ypos, t_floatarg
shift, t_floatarg ctrl, t_floatarg alt)
{
    using namespace std::chrono;

    milliseconds ms = duration_cast< milliseconds
>(system_clock::now().time_since_epoch());
    milliseconds diff = (ms - x->m_last_click);

    // 200 ms double click
    if (diff.count() < 200)
    {
        x->m_last_click = milliseconds(0);

        int l;
        t_device *device = (t_device*) find_parent(&x->m_obj,"ossia.device", 0,
&l);
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
    {
        x->m_last_click = ms;
    }
}
 */

namespace ossia
{
namespace max
{
bool t_view::register_node(ossia::net::node_base* node)
{
  bool res = do_registration(node);

  if (res)
  {
    object_dequarantining<t_view>(this);

    std::vector<t_object_base*> children_view = find_children_to_register(
        &m_object, get_patcher(&m_object), gensym("ossia.view"));

    for (auto child : children_view)
    {
      if (child->m_otype == Type::view)
      {
        t_view* view = (t_view*)child;

        if (view == this)
        {
          // not registering itself
          continue;
        }

        view->register_node(m_node);
      }
      else if (child->m_otype == Type::remote)
      {
        t_remote* remote = (t_remote*)child;
        remote->register_node(m_node);
      }
    }
  }
  else
    object_quarantining<t_view>(this);

  return res;
}

bool t_view::do_registration(ossia::net::node_base* node)
{
  // already register to this node;
  if (m_node && m_node->get_parent() == node)
    return true;

  // we should unregister here because we may have add a node between the
  // registered node and the remote
  unregister();

  if (node)
  {
    std::string absolute_path = object_path_absolute<t_view>(this);
    std::string address_string = ossia::net::address_string_from_node(*node);

    if (absolute_path != address_string)
      return false;

    m_node = node->find_child(m_name->s_name);

    if (m_node)
      m_node->about_to_be_deleted.connect<t_view, &t_view::is_deleted>(this);
    else
      return false;
  }
  else
    return false;

  return true;
}

void t_view::register_children(t_view* x)
{
  std::vector<t_object_base*> children_view = find_children_to_register(
      &x->m_object, get_patcher(&m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == Type::view)
    {
      t_view* view = (t_view*)child;

      if (view == x)
        continue;

      max_object_register<t_view>(view);
    }
    else if (child->m_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)child;
      max_object_register<t_remote>(remote);
    }
  }
}

bool t_view::unregister()
{
  // not registered
  if (!m_node)
    return true;

  std::vector<t_object_base*> children_view = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == Type::view)
    {
      t_view* view = (t_view*)child;

      if (view == this)
        continue;

      view->unregister();
    }
    else if (child->m_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)child;
      remote->unregister();
    }
  }

  m_node = nullptr;
  object_quarantining<t_view>(this);

  register_children(this);

  return true;
}
}
}
