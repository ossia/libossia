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

  t_symbol* x_type;
  int x_type_size;
  t_atom x_default[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom x_min[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom x_max[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom x_range[OSSIA_PD_MAX_ATTR_SIZE];
  t_symbol* x_bounding_mode;
  t_symbol* x_access_mode;
  t_float x_repetition_filter = 0;
  t_symbol* x_unit;
  t_symbol* x_tags;
  t_symbol* x_description;
  int x_priority;
  bool x_hidden;
  bool x_enable;

  static ossia::safe_vector<t_param*>& quarantine();

  void is_deleted(const ossia::net::node_base& n);


};
}
} // namespace
