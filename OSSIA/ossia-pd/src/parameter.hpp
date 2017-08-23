#pragma once

#include "device.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_param : t_obj_base
{
  using is_model = std::true_type;
  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  // attribute setting method
  void set_access_mode();
  void set_repetition_filter();
  void set_description();
  void set_tags();
  void set_priority();
  void set_enable();
  void set_hidden();
  void set_minmax();
  void set_range();
  void set_bounding_mode();
  void set_default();
  void set_unit();
  void set_type();

  // attributes
  t_symbol* x_type;
  t_atom x_default[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom x_min[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom x_max[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom x_range[OSSIA_PD_MAX_ATTR_SIZE];
  t_symbol* x_bounding_mode;
  t_symbol* x_access_mode;
  t_float x_repetition_filter = 0;
  t_symbol* x_unit;
  t_atom x_tags[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom x_description[OSSIA_PD_MAX_ATTR_SIZE];
  int x_priority;
  bool x_hidden;
  bool x_enable;

  // size of size-variable attribute
  long x_default_size;
  long x_range_size;
  long x_min_size;
  long x_max_size;
  long x_tags_size;
  long x_description_size;

  static ossia::safe_vector<t_param*>& quarantine();

  void is_deleted(const ossia::net::node_base& n);


};
}
} // namespace
