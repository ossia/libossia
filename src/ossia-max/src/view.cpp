// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "view.hpp"

#include "remote.hpp"
#include "utils.hpp"

using namespace ossia::max_binding;

#pragma mark -
#pragma mark ossia_remote class methods

extern "C" void ossia_view_setup()
{
  // instantiate the ossia.view class
  t_class* c = class_new(
      "ossia.view", (method)view::create, (method)view::destroy, (short)sizeof(view), 0L,
      A_GIMME, 0);

  if(c)
  {
    node_base::class_setup(c);

    class_addmethod(c, (method)address_mess_cb<view>, "address", A_SYM, 0);
    class_addmethod(c, (method)view::get_mess_cb, "get", A_SYM, 0);

    class_addmethod(c, (method)view::notify, "notify", A_CANT, 0);
  }

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_view_class = c;
}

namespace ossia
{
namespace max_binding
{

void* view::create(t_symbol* name, long argc, t_atom* argv)
{
  auto x = make_ossia<view>();

  if(x)
  {
    critical_enter(0);
    auto& pat_desc = ossia_max::get_patcher_descriptor(x->m_patcher);
    if(!pat_desc.model && !pat_desc.view)
    {
      pat_desc.view = x;
    }
    else
    {
      error("You can put only one [ossia.model] or [ossia.view] per patcher");
      object_free(x);
      critical_exit(0);
      return nullptr;
    }
    device_base::on_device_created.connect<&view::on_device_created>(x);
    device_base::on_device_removing.connect<&view::on_device_removing>(x);

    std::vector<ossia::net::generic_device*> devs = get_all_devices();
    for(auto dev : devs)
    {
      if(!x->m_devices.contains(dev))
      {
        dev->on_node_created.connect<&view::on_node_created_callback>(x);
        dev->on_node_renamed.connect<&view::on_node_renamed_callback>(x);
        x->m_devices.push_back(dev);
      }
    }

    x->m_otype = object_class::view;

    // make outlets
    x->m_dumpout = outlet_new(x, NULL); // anything outlet to dump view state

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if(attrstart && argv)
    {
      if(atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    defer_low(x, (method)object_base::loadbang, nullptr, 0, nullptr);

    ossia_max::instance().views.push_back(x);
    critical_exit(0);
  }

  return (x);
}

void view::destroy(view* x)
{
  critical_enter(0);

  for(auto dev : get_all_devices())
  {
    dev->on_node_created.disconnect<&view::on_node_created_callback>(x);
    dev->on_node_renamed.disconnect<&view::on_node_renamed_callback>(x);
  }
  x->m_devices.clear();

  device_base::on_device_created.disconnect<&view::on_device_created>(x);
  device_base::on_device_removing.disconnect<&view::on_device_removing>(x);

  x->m_dead = true;
  x->unregister();
  ossia_max::instance().views.remove_all(x);
  if(x->m_clock)
    object_free(x->m_clock);
  if(x->m_dumpout)
    outlet_delete(x->m_dumpout);
  x->~view();
  critical_exit(0);
}

void view::do_registration()
{
  if(m_name && std::string_view(m_name->s_name) != "")
  {
    m_registered = true;

    clear_and_init_registration();
  }
}

void view::unregister()
{
  m_node_selection.clear();
  m_matchers.clear();

  std::vector<object_base*> children_view
      = find_children_to_register(&m_object, m_patcher, gensym("ossia.view"));

  for(auto child : children_view)
  {
    switch(child->m_otype)
    {
      case object_class::view: {
        ossia::max_binding::view* view = (ossia::max_binding::view*)child;

        if(view == this)
          continue;

        view->unregister();
        break;
      }
      case object_class::remote: {
        ossia::max_binding::remote* remote = (ossia::max_binding::remote*)child;
        remote->unregister();
        break;
      }
      case object_class::attribute: {
        ossia::max_binding::attribute* attr = (ossia::max_binding::attribute*)child;
        attr->unregister();
        break;
      }
      default:
        break;
    }
  }

  m_registered = false;
}

void view::on_node_renamed_callback(ossia::net::node_base& node, const std::string&)
{
  // first remove the matcher with old name
  remove_matchers(node);

  // try to find a new match for the new name
  on_node_created_callback(node);
}

void view::on_node_created_callback(ossia::net::node_base& node)
{
  if(!ossia_max::instance().config.autoregister)
    return;

  do_registration();

  // update all children objects
  register_children_in_patcher_recursively(m_patcher, this);
}

void view::on_device_removing(device_base* obj)
{
  auto dev = obj->m_device.get();
  if(m_devices.contains(dev))
  {
    dev->on_node_created.disconnect<&view::on_node_created_callback>(this);
    dev->on_node_renamed.disconnect<&view::on_node_renamed_callback>(this);

    m_devices.remove_all(dev);
  }
}

void view::on_device_created(device_base* obj)
{
  auto dev = obj->m_device.get();
  if(!m_devices.contains(dev))
  {
    // no need to connect to on_node_removing because ossia::max::matcher
    // already connect to it
    dev->on_node_created.connect<&view::on_node_created_callback>(this);
    dev->on_node_renamed.connect<&view::on_node_renamed_callback>(this);

    m_devices.push_back(dev);
  }
}

} // max namespace
} // ossia namespace
