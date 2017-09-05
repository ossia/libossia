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
  t_symbol* m_tags[64];
  t_symbol* m_description;
  int m_priority;
  int m_hidden;

  long m_tags_size;

  bool register_node(const std::vector<ossia::net::node_base*>& nodes);
  bool do_registration(const std::vector<ossia::net::node_base*>& nodes);
  bool unregister();
  void register_children();

  void is_deleted(const ossia::net::node_base&);

  static ossia::safe_vector<t_model*>& quarantine();

  void set_tags();
  void set_description();
  void set_priority();
};

} // max namespace
} // ossia namespace

extern "C" {
void* ossia_model_new(t_symbol*, long, t_atom*);
void ossia_model_assist(ossia::max::t_model*, void*, long, long, char*);
void ossia_model_free(ossia::max::t_model*);
}
