// SPDX-FileCopyrightText: 2026 CNRS
// SPDX-FileContributor: Julien Bayle, PRISM <https://www.prism.cnrs.fr/>

#pragma once

// ossia.v8connector: a node of the ossia tree whose parameters are declared
// by messages, typically from a v8 or v8ui script.
//
//   [ossia.v8connector <branch>]
//   add <address> @type ... @min ... @max ...   create a parameter under the branch
//   remove <address>                              remove it
//   clear                                         remove every parameter of this instance
//   bind <address>                                follow a parameter declared elsewhere,
//                                                 addressed as an ossia.remote would
//   unbind <address>                              stop following it
//   set <address> <values...>                     push a value to a leaf or a bound parameter
//   left outlet                                   <address> <values...>, for values that
//                                                 come from any other source; "ready" once
//                                                 the instance has registered
//   right outlet                                  status and errors
//
// The node registers like an ossia.model: under the ossia.model, ossia.device
// or ossia.client found in the patcher hierarchy, or under the global device.
// It is created by the first add and stays until the object is deleted, like
// an ossia.model without children; an instance that only binds creates nothing,
// like an ossia.remote. Duplicate names get an instance suffix, as every ossia
// node.

#include <ossia-max/src/node_base.hpp>
#include <ossia-max/src/v8connector_branch.hpp>

#include <vector>

#include <ossia-max_export.h>

extern "C" {
OSSIA_MAX_EXPORT void ossia_v8connector_setup();
}

namespace ossia
{
namespace max_binding
{

class device_base;

class v8connector : public node_base
{
public:
  static t_class* max_class;

  v8connector_detail::branch m_branch;
  void* m_queue_qelem{};
  void* m_register_qelem{};
  std::vector<t_atom> m_atoms;
  // @bundle: the sets of a tick leave together, in OSC bundles (1, default),
  // or one message per value (0).
  long m_bundle{1};

  void do_registration();
  void unregister();
  ossia::net::node_base* create_branch_node();
  void schedule_registration();
  void report_error(const std::string& message);
  void status(const char* what, const std::string& argument);

  void on_device_created(device_base* dev);
  void on_device_removing(device_base* dev);
  void update_resolution_context();
  ossia::net::node_base* shared_node();
  bool has_name() const { return m_name && m_name != _sym_nothing; }

  static void* create(t_symbol*, long argc, t_atom* argv);
  static void destroy(v8connector* x);
  static void assist(v8connector* x, void* b, long m, long a, char* s);
  static void loadbang(v8connector* x);

  static void add(v8connector* x, t_symbol* s, long argc, t_atom* argv);
  static void remove(v8connector* x, t_symbol* s, long argc, t_atom* argv);
  static void clear(v8connector* x);
  static void set(v8connector* x, t_symbol* s, long argc, t_atom* argv);
  static void bind(v8connector* x, t_symbol* s);
  static void unbind(v8connector* x, t_symbol* s);
  static void address(v8connector* x, t_symbol* s);

  static void drain(v8connector* x);
  static void deferred_registration(v8connector* x);
};

} // namespace max_binding
} // namespace ossia
