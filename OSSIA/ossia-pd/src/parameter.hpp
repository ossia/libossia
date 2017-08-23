#pragma once

#include "device.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "ossia_obj_base.hpp"

#define OSSIA_PD_MAX_ATTR_SIZE 64

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
  void parameter_set_access_mode();
  void parameter_set_repetition_filter();
  void parameter_set_description();
  void parameter_set_tags();
  void parameter_set_priority();
  void parameter_set_enable();
  void parameter_set_hidden();
  void parameter_set_minmax();
  void parameter_set_range();
  void parameter_set_bounding_mode();
  void parameter_set_default();
  void parameter_set_unit();
  void parameter_set_type();

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
