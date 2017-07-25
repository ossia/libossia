#pragma once

#include "ossia_object_base.hpp"

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_model structure declaration

struct t_model : t_object_base
{
  t_symbol* m_tags;
  t_symbol* m_description;

  bool register_node(ossia::net::node_base*);
  bool do_registration(ossia::net::node_base*);
  bool unregister();
  void register_children();

  void is_deleted(const ossia::net::node_base&);

  static ossia::safe_vector<t_model*>& quarantine();
};

} // max namespace
} // ossia namespace

extern "C" {
void* ossia_model_new(t_symbol*, long, t_atom*);
void ossia_model_assist(ossia::max::t_model*, void*, long, long, char*);
void ossia_model_free(ossia::max::t_model*);
}
