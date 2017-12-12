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
    // make outlets
    x->m_dumpout = outlet_new(x, NULL); // anything outlet to dump view state

    //        x->m_clock = clock_new(x, (method)object_base::tick);
    x->m_clock = clock_new(
        x, reinterpret_cast<method>(
               static_cast<bool (*)(view*)>(&max_object_register<view>)));

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);
    x->m_otype = object_class::view;

    if(find_peer(x))
    {
      error("You can put only one [ossia.model] or [ossia.view] per patcher");
      view::destroy(x);
      return nullptr;
    }

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }

      // we need to delay registration because object may use patcher hierarchy
      // to check address validity
      // and object will be added to patcher's objects list (aka canvas g_list)
      // after model_new() returns.
      // 0 ms delay means that it will be perform on next clock tick
      clock_delay(x->m_clock, 0);
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

  if (x) ossia_max::instance().views.push_back(x);
  return (x);
}

void view::destroy(view* x)
{
  x->m_dead = true;
  x->unregister();
  object_dequarantining<view>(x);
  ossia_max::instance().views.remove_all(x);
  object_free(x->m_clock);

  if(x->m_dumpout) outlet_delete(x->m_dumpout);
  x->~view();
}

bool view::register_node(const std::vector<ossia::net::node_base*>& nodes)
{
  bool res = do_registration(nodes);

  if (res)
  {
    object_dequarantining<view>(this);
    std::vector<object_base*> children_view = find_children_to_register(
        &m_object, get_patcher(&m_object), gensym("ossia.view"));

    for (auto child : children_view)
    {
      if (child->m_otype == object_class::view)
      {
        ossia::max::view* view = (ossia::max::view*)child;
        view->register_node(m_nodes);
      }
      else if (child->m_otype == object_class::remote)
      {
        ossia::max::remote* remote = (ossia::max::remote*)child;
        remote->register_node(m_nodes);
      }
    }
  }
  else
    object_quarantining<ossia::max::view>(this);

  return res;
}

bool view::do_registration(const std::vector<ossia::net::node_base*>& _nodes)
{
  // we should unregister here because we may have add a node between the
  // registered node and the remote
  unregister();

  for (auto _node : _nodes)
  {
    std::string name = m_name->s_name;

    if (m_addr_scope == ossia::net::address_scope::absolute)
    {
      // get root node
      _node = &_node->get_device().get_root_node();
      // and remove starting '/'
      name = name.substr(1);
    }

    m_parent_node = _node;

    std::vector<ossia::net::node_base*> nodes{};

    if (m_addr_scope == ossia::net::address_scope::global)
      nodes = ossia::max::find_global_nodes(name);
    else
      nodes = ossia::net::find_nodes(*_node, name);

    m_nodes.reserve(m_nodes.size() + nodes.size());
    m_matchers.reserve(m_matchers.size() + nodes.size());

    for (auto n : nodes)
    {
      // we may have found a node with the same name
      // but with a parameter, in that case it's an ø.param
      // then forget it
      if (!n->get_parameter())
        m_matchers.emplace_back(n, this);
      m_nodes.push_back(n);
    }
  }

  fill_selection();

  return (!m_matchers.empty() || m_is_pattern);
}

void view::register_children(view* x)
{
  std::vector<object_base*> children_view = find_children_to_register(
      &x->m_object, get_patcher(&x->m_object), gensym("ossia.view"));

  for (auto child : children_view)
  {
    if (child->m_otype == object_class::view)
    {
      ossia::max::view* view = (ossia::max::view*)child;

      if (view == x)
        continue;

      max_object_register<ossia::max::view>(view);
    }
    else if (child->m_otype == object_class::remote)
    {
      ossia::max::remote* remote = (ossia::max::remote*)child;
      max_object_register<ossia::max::remote>(remote);
    }
  }
}

bool view::unregister()
{
  m_matchers.clear();
  m_nodes.clear();

  std::vector<object_base*> children_view = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.view"));

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

  object_quarantining<view>(this);

  register_children(this);

  return true;
}

ossia::safe_set<view*>& view::quarantine()
{
    return ossia_max::instance().view_quarantine;
}

} // max namespace
} // ossia namespace
