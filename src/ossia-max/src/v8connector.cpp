// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>
// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "v8connector.hpp"

#include "device_base.hpp"
#include "ossia-max.hpp"
#include "utils.hpp"

#include <ossia/network/base/node_functions.hpp>

#include <algorithm>

using namespace ossia::max_binding;
namespace v8d = ossia::max_binding::v8connector_detail;

t_class* v8connector::max_class = nullptr;

extern "C" void ossia_v8connector_setup()
{
  t_class* c = class_new(
      "ossia.v8connector", (method)v8connector::create, (method)v8connector::destroy,
      (long)sizeof(v8connector), 0L, A_GIMME, 0);

  // Node attributes, applied to the branch node as ossia.model does.
  CLASS_ATTR_FLOAT(c, "priority", 0, object_base, m_priority);
  CLASS_ATTR_LABEL(c, "priority", 0, "Priority");

  CLASS_ATTR_SYM(c, "description", 0, object_base, m_description);
  CLASS_ATTR_LABEL(c, "description", 0, "Description");

  CLASS_ATTR_SYM_VARSIZE(
      c, "tags", 0, object_base, m_tags, m_tags_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "tags", 0, "Tags");

  CLASS_ATTR_LONG(c, "invisible", 0, object_base, m_invisible);
  CLASS_ATTR_STYLE(c, "invisible", 0, "onoff");
  CLASS_ATTR_LABEL(c, "invisible", 0, "Invisible");

  CLASS_ATTR_LONG(c, "recall_safe", 0, object_base, m_recall_safe);
  CLASS_ATTR_STYLE(c, "recall_safe", 0, "onoff");
  CLASS_ATTR_LABEL(c, "recall_safe", 0, "Recall safe");

  CLASS_ATTR_LONG(c, "bundle", 0, v8connector, m_bundle);
  CLASS_ATTR_STYLE(c, "bundle", 0, "onoff");
  CLASS_ATTR_LABEL(
      c, "bundle", 0, "Send the values set during a tick together, in OSC bundles");

  // No @trim_address: the addresses this object outputs are always the ones
  // declared or bound, and it never read the attribute it showed
  class_addmethod(c, (method)v8connector::add, "add", A_GIMME, 0);
  class_addmethod(c, (method)v8connector::remove, "remove", A_GIMME, 0);
  class_addmethod(c, (method)v8connector::clear, "clear", A_NOTHING, 0);
  class_addmethod(c, (method)v8connector::set, "set", A_GIMME, 0);
  class_addmethod(c, (method)v8connector::bind, "bind", A_SYM, 0);
  class_addmethod(c, (method)v8connector::unbind, "unbind", A_SYM, 0);
  class_addmethod(c, (method)v8connector::address, "address", A_SYM, 0);
  class_addmethod(c, (method)node_base::get_namespace, "namespace", A_GIMME, 0);
  class_addmethod(c, (method)object_base::get_mess_cb, "get", A_SYM, 0);
  class_addmethod(c, (method)v8connector::loadbang, "loadbang", A_CANT, 0);
  class_addmethod(c, (method)object_base::notify, "notify", A_CANT, 0);
  class_addmethod(c, (method)v8connector::assist, "assist", A_CANT, 0);

  class_register(CLASS_BOX, c);
  v8connector::max_class = c;
}

namespace ossia
{
namespace max_binding
{

namespace
{

v8d::scalar atom_to_scalar(const t_atom& a)
{
  switch(atom_gettype(&a))
  {
    case A_LONG:
      return static_cast<int>(atom_getlong(&a));
    case A_FLOAT:
      return static_cast<float>(atom_getfloat(&a));
    case A_SYM:
      return std::string(atom_getsym(&a)->s_name);
    default:
      return 0;
  }
}

bool is_attribute_name(const t_atom& a)
{
  return atom_gettype(&a) == A_SYM && atom_getsym(&a)->s_name[0] == '@'
         && atom_getsym(&a)->s_name[1] != '\0';
}

}

void* v8connector::create(t_symbol*, long argc, t_atom* argv)
{
  auto x = make_ossia<v8connector>();
  if(!x)
    return nullptr;

  critical_enter(0);

  // Right outlet: status and errors. Left outlet: values.
  x->m_dumpout = outlet_new(x, nullptr);
  x->m_data_out = outlet_new(x, nullptr);

  x->m_description = gensym("");
  x->m_tags_size = 0;

  long attrstart = attr_args_offset(argc, argv);
  x->m_name = _sym_nothing;
  if(attrstart > 0 && argv && atom_gettype(argv) == A_SYM)
  {
    x->m_name = atom_getsym(argv);
    x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
  }
  attr_args_process(x, argc - attrstart, argv + attrstart);

  // Without a name, the parameters sit directly under the enclosing model or
  // device, next to the ossia.parameter objects.
  x->m_branch.set_owns_node(x->has_name());

  x->m_queue_qelem = qelem_new(x, (method)v8connector::drain);
  x->m_register_qelem = qelem_new(x, (method)v8connector::deferred_registration);

  // Both callbacks may fire from the thread that changed the tree or pushed a
  // value: they only schedule work on the main thread.
  void* queue = x->m_queue_qelem;
  x->m_branch.set_wake_callback([queue] { qelem_set(queue); });
  x->m_branch.set_node_lost_callback([x] { x->schedule_registration(); });
  x->m_branch.set_node_factory([x]() -> ossia::net::node_base* {
    return x->m_dead ? nullptr : x->create_branch_node();
  });
  x->m_branch.set_binding_changed_callback(
      [x](const std::string& address, std::size_t n) {
    if(x->m_dead)
      return;
    t_atom a[2];
    A_SETSYM(a, gensym(address.c_str()));
    A_SETLONG(a + 1, static_cast<long>(n));
    outlet_anything(x->m_dumpout, gensym("bind"), 2, a);
  });

  device_base::on_device_created.connect<&v8connector::on_device_created>(x);
  device_base::on_device_removing.connect<&v8connector::on_device_removing>(x);

  // Receive notifications when an attribute changes
  object_attach_byptr_register(x, x, CLASS_BOX);

  defer_low(x, (method)v8connector::loadbang, nullptr, 0, nullptr);

  critical_exit(0);
  return x;
}

void v8connector::destroy(v8connector* x)
{
  critical_enter(0);
  x->m_dead = true;

  device_base::on_device_created.disconnect<&v8connector::on_device_created>(x);
  device_base::on_device_removing.disconnect<&v8connector::on_device_removing>(x);

  x->m_branch.set_wake_callback({});
  x->m_branch.set_node_lost_callback({});
  x->m_branch.set_binding_changed_callback({});
  x->m_branch.set_node_factory({});
  x->unregister();

  if(x->m_register_qelem)
    qelem_free(x->m_register_qelem);
  if(x->m_queue_qelem)
    qelem_free(x->m_queue_qelem);

  outlet_delete(x->m_data_out);
  outlet_delete(x->m_dumpout);
  x->~v8connector();
  critical_exit(0);
}

void v8connector::assist(v8connector*, void*, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "add, remove, clear, bind, unbind, set <address> <values...>");
  }
  else if(a == 0)
  {
    sprintf(s, "<address> <values...> changed by another source, ready at registration");
  }
  else
  {
    sprintf(s, "Dumpout: status and errors");
  }
}

void v8connector::report_error(const std::string& message)
{
  object_error((t_object*)this, "%s", message.c_str());
  t_atom a;
  A_SETSYM(&a, gensym(message.c_str()));
  outlet_anything(m_dumpout, gensym("error"), 1, &a);
}

void v8connector::status(const char* what, const std::string& argument)
{
  if(argument.empty())
  {
    outlet_anything(m_dumpout, gensym(what), 0, nullptr);
  }
  else
  {
    t_atom a;
    A_SETSYM(&a, gensym(argument.c_str()));
    outlet_anything(m_dumpout, gensym(what), 1, &a);
  }
}

void v8connector::loadbang(v8connector* x)
{
  if(!ossia_max::instance().config.autoregister)
    return;
  if(x->m_registered || x->m_dead)
    return;
  critical_enter(0);
  // Registers the whole patcher hierarchy if it has not been done yet, so
  // that the enclosing models and devices exist before the branch is created.
  object_base::loadbang(x);
  if(!x->m_registered)
    x->do_registration();
  critical_exit(0);
}

void v8connector::do_registration()
{
  if(m_registering)
    return;
  m_registering = true;

  unregister();
  // address <name> may have changed the mode: the branch has left its old node.
  m_branch.set_owns_node(has_name());

  if(has_name())
    update_path();

  m_registered = true;
  update_resolution_context();
  // The node is created only if there are leaves to put under it.
  m_branch.reattach();

  // Tells the script this instance is ready for its declarations: a new
  // instance (undo of a deletion, paste) has none, the script replays them.
  outlet_anything(m_data_out, gensym("ready"), 0, nullptr);

  m_registering = false;
}

// Create the node of this instance under its parent, like an ossia.model does:
// a node that already exists gets an instance suffix. Returns nullptr when no
// device matches.
ossia::net::node_base* v8connector::create_branch_node()
{
  if(m_registered && !has_name())
    return shared_node();
  if(!m_registered || !has_name() || !m_matchers.empty())
    return m_matchers.empty() ? nullptr : m_matchers.front()->get_node();

  auto devices = get_all_devices();
  for(const std::string& path : m_paths)
  {
    const auto pos = path.find(":/");
    if(pos == std::string::npos)
      continue;
    const std::string_view prefix(path.data(), pos);
    const std::string_view osc_address(path.data() + pos + 1);

    for(auto dev : devices)
    {
      if(dev->get_name() != prefix)
        continue;
      auto& node = ossia::net::create_node(dev->get_root_node(), osc_address);
      m_matchers.push_back(std::make_shared<matcher>(&node, this));
    }
  }

  set_matchers_index();
  m_selection_path.reset();
  fill_selection();

  if(m_matchers.empty())
  {
    object_error(&m_object, "Can't find a device for %s", m_name->s_name);
    return nullptr;
  }

  set_priority();
  set_description();
  set_tags();
  set_hidden();
  set_recall_safe();

  if(m_matchers.size() > 1)
    object_warn(
        &m_object, "%s matches several nodes, the parameters are created under %s",
        m_name->s_name,
        ossia::net::address_string_from_node(*m_matchers.front()->get_node()).c_str());

  auto* node = m_matchers.front()->get_node();
  status("address", ossia::net::address_string_from_node(*node));
  return node;
}

// Without a name: the node of the enclosing model, or the root of the
// enclosing device or client, or the root of the global device. It belongs to
// that object; the branch only adds its leaves to it.
ossia::net::node_base* v8connector::shared_node()
{
  ossia::net::node_base* node{};
  if(auto* parent = find_parent_object())
  {
    switch(parent->m_otype)
    {
      case object_class::device:
      case object_class::client:
        if(auto dev = static_cast<device_base*>(parent)->m_device)
          node = &dev->get_root_node();
        break;
      case object_class::model:
        for(const auto& m : parent->m_matchers)
          if(auto n = m->get_node(); n && !m->is_dead() && !m->is_zombie())
          {
            node = n;
            break;
          }
        break;
      default:
        break;
    }
  }
  if(!node)
    node = &ossia_max::instance().get_default_device()->get_root_node();
  status("address", ossia::net::address_string_from_node(*node));
  return node;
}

// Bound addresses resolve like an ossia.remote placed next to this object:
// relative to the nodes of the enclosing model or view, or to the root of the
// enclosing device, or to the global device.
void v8connector::update_resolution_context()
{
  std::vector<ossia::net::node_base*> parents;
  if(auto* parent = find_parent_object())
  {
    switch(parent->m_otype)
    {
      case object_class::device:
      case object_class::client:
        if(auto dev = static_cast<device_base*>(parent)->m_device)
          parents.push_back(&dev->get_root_node());
        break;
      case object_class::model:
      case object_class::view:
        for(const auto& m : parent->m_matchers)
          if(auto n = m->get_node(); n && !m->is_dead() && !m->is_zombie())
            parents.push_back(n);
        break;
      default:
        break;
    }
  }
  if(parents.empty())
    parents.push_back(&ossia_max::instance().get_default_device()->get_root_node());

  std::vector<ossia::net::device_base*> devices;
  for(auto dev : get_all_devices())
    devices.push_back(dev);

  m_branch.set_resolution_context(std::move(parents), std::move(devices));
}

void v8connector::unregister()
{
  m_branch.detach();

  std::vector<ossia::net::node_base*> nodes;
  nodes.reserve(m_matchers.size());
  for(const auto& m : m_matchers)
    if(auto n = m->get_node(); n && !m->is_dead() && !m->is_zombie())
      nodes.push_back(n);

  m_node_selection.clear();
  m_matchers.clear();

  for(auto n : nodes)
    if(auto parent = n->get_parent())
      parent->remove_child(*n);

  m_registered = false;
}

void v8connector::schedule_registration()
{
  if(!m_dead && m_register_qelem)
    qelem_set(m_register_qelem);
}

void v8connector::deferred_registration(v8connector* x)
{
  if(x->m_dead || !x->m_registered)
    return;
  critical_enter(0);
  x->do_registration();
  critical_exit(0);
}

void v8connector::on_device_created(device_base* dev)
{
  if(m_dead || !m_registered || !dev)
    return;
  // Only a device in the patcher hierarchy changes where the branch belongs.
  get_hierarchy();
  if(std::find(m_patcher_hierarchy.begin(), m_patcher_hierarchy.end(), dev->m_patcher)
     != m_patcher_hierarchy.end())
    schedule_registration();
}

void v8connector::on_device_removing(device_base* dev)
{
  if(dev && dev->m_device)
    m_branch.drop_device(dev->m_device.get());
}

void v8connector::bind(v8connector* x, t_symbol* s)
{
  if(!s || s == _sym_nothing)
  {
    x->m_branch.refuse("", "bind expects an address");
    x->report_error("bind expects an address");
    return;
  }
  if(auto err = x->m_branch.bind(s->s_name); !err.empty())
    x->report_error("bind refused: " + err);
}

void v8connector::unbind(v8connector* x, t_symbol* s)
{
  if(!s)
  {
    x->m_branch.refuse("", "unbind expects an address");
    x->report_error("unbind expects an address");
    return;
  }
  if(!x->m_branch.unbind(s->s_name))
  {
    x->report_error(
        std::string("unbind: unknown address '") + (s ? s->s_name : "") + "'");
    return;
  }
  x->status("unbind", s->s_name);
}

void v8connector::add(v8connector* x, t_symbol*, long argc, t_atom* argv)
{
  if(argc < 1 || atom_gettype(argv) != A_SYM)
  {
    x->m_branch.refuse("", "add expects an address followed by @attributes");
    x->report_error("add expects an address followed by @attributes");
    return;
  }

  std::vector<v8d::attribute> attrs;
  for(long i = 1; i < argc; ++i)
  {
    if(is_attribute_name(argv[i]))
    {
      attrs.push_back({std::string(atom_getsym(argv + i)->s_name + 1), {}});
    }
    else if(attrs.empty())
    {
      x->m_branch.refuse(
          atom_getsym(argv)->s_name, "unexpected argument before the first @attribute");
      x->report_error(
          std::string("add ") + atom_getsym(argv)->s_name
          + ": unexpected argument before the first @attribute");
      return;
    }
    else
    {
      attrs.back().values.push_back(atom_to_scalar(argv[i]));
    }
  }

  // declare validates, adds, and queues the refusal for the left outlet.
  if(auto err = x->m_branch.declare(atom_getsym(argv)->s_name, attrs); !err.empty())
  {
    x->report_error("add refused: " + err);
    return;
  }
  x->status("add", v8d::normalize_relative_address(atom_getsym(argv)->s_name));
}

void v8connector::remove(v8connector* x, t_symbol*, long argc, t_atom* argv)
{
  if(argc < 1 || atom_gettype(argv) != A_SYM)
  {
    x->m_branch.refuse("", "remove expects an address");
    x->report_error("remove expects an address");
    return;
  }
  const std::string address = atom_getsym(argv)->s_name;
  if(!x->m_branch.remove(address))
  {
    x->report_error("remove: unknown address '" + address + "'");
    return;
  }
  x->status("remove", v8d::normalize_relative_address(address));
}

void v8connector::clear(v8connector* x)
{
  x->m_branch.clear();
  x->status("clear", {});
}

void v8connector::set(v8connector* x, t_symbol*, long argc, t_atom* argv)
{
  if(argc < 1 || atom_gettype(argv) != A_SYM)
  {
    x->m_branch.refuse("", "set expects an address followed by the value");
    x->report_error("set expects an address followed by the value");
    return;
  }
  v8d::scalars values;
  values.reserve(argc - 1);
  for(long i = 1; i < argc; ++i)
    values.push_back(atom_to_scalar(argv[i]));

  // @bundle may have changed since the last set; turned off, the values
  // queued so far leave first.
  x->m_branch.set_bundle(x->m_bundle > 0);
  if(auto err = x->m_branch.set(atom_getsym(argv)->s_name, values); !err.empty())
    x->report_error("set: " + err);
}

void v8connector::address(v8connector* x, t_symbol* s)
{
  if(!s || s == _sym_nothing)
    return;
  x->m_name = s;
  x->m_addr_scope = ossia::net::get_address_scope(s->s_name);
  if(x->m_registered)
  {
    critical_enter(0);
    x->do_registration();
    critical_exit(0);
  }
}

// Main thread only: output the values received since the last call.
void v8connector::drain(v8connector* x)
{
  if(x->m_dead)
    return;
  // The end of the tick: what was set since the last drain leaves together.
  x->m_branch.flush();
  for(const auto& in : x->m_branch.drain())
  {
    x->m_atoms.clear();
    value2atom vm{x->m_atoms};
    in.value.apply(vm);
    outlet_anything(
        x->m_data_out, gensym(in.address.c_str()), static_cast<short>(x->m_atoms.size()),
        x->m_atoms.data());
  }
}

} // namespace max_binding
} // namespace ossia
