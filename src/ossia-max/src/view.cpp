// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "view.hpp"
#include "remote.hpp"
#include "utils.hpp"

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_remote class methods

extern "C" void ossia_view_setup()
{
  // instantiate the ossia.view class
  t_class* c = class_new(
      "ossia.view", (method)view::create, (method)view::destroy,
      (short)sizeof(view), 0L, A_GIMME, 0);

  if (c)
  {
    class_addmethod(c, (method) address_mess_cb<view>, "address",   A_SYM, 0);
    class_addmethod(c, (method) view::get_mess_cb, "get",   A_SYM, 0);

    class_addmethod(
          c, (method)view::notify,
          "notify", A_CANT, 0);
  }

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia_max::instance();
  ossia_library.ossia_view_class = c;
}

namespace ossia
{
namespace max
{

void* view::create(t_symbol* name, long argc, t_atom* argv)
{
  auto x = make_ossia<view>();

  if (x)
  {
    critical_enter(0);
    auto& pat_desc = ossia_max::get_patcher_descriptor(x->m_patcher);
    if( !pat_desc.model && !pat_desc.view)
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

    x->m_otype = object_class::view;

    // make outlets
    x->m_dumpout = outlet_new(x, NULL); // anything outlet to dump view state

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    defer_low(x, (method) object_base::loadbang, nullptr, 0, nullptr);

    ossia_max::instance().views.push_back(x);
    critical_exit(0);
  }

  return (x);
}

void view::destroy(view* x)
{
  critical_enter(0);
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
  m_registered = true;

  m_matchers = find_or_create_matchers();

  m_selection_path.reset();
  fill_selection();
}

void view::unregister()
{
  m_node_selection.clear();
  m_matchers.clear();

  std::vector<object_base*> children_view = find_children_to_register(
      &m_object, m_patcher, gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == object_class::view)
    {
      ossia::max::view* view = (ossia::max::view*)child;

      if (view == this)
        continue;

      view->unregister();
    }
    else if (child->m_otype == object_class::remote)
    {
      ossia::max::remote* remote = (ossia::max::remote*)child;
      remote->unregister();
    }
  }

  ossia_max::instance().nr_views.push_back(this);

  m_registered = false;
}

void view::on_node_created_callback(ossia::net::node_base& node)
{
  auto oscaddr = ossia::net::address_string_from_node(node);

  if ( ossia::traversal::match(get_path(), node) )
  {
    m_matchers.emplace_back(std::make_shared<matcher>(&node,this));
    fill_selection();
  }
}

void view::on_device_removing(device_base* obj)
{
  auto dev = obj->m_device.get();
  if(m_devices.contains(dev))
  {
    dev->on_node_created.disconnect<&view::on_node_created_callback>(this);
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
    m_devices.push_back(dev);
  }
}

} // max namespace
} // ossia namespace
