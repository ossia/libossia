#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_model : t_obj_base
{
  using is_model = std::true_type;

  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();
  void register_children();

  static ossia::safe_vector<t_model*>& quarantine();

  void is_deleted(const ossia::net::node_base& n);

  void set_tags();
  void set_description();
  void set_priority();

  t_atom x_tags[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom x_description[OSSIA_PD_MAX_ATTR_SIZE];
  int x_priority;

  long x_tags_size;
  long x_description_size;
};
}
} // namespace
