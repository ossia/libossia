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

  static ossia::safe_vector<t_model*>& quarantine();
  static ossia::safe_vector<t_model*>& rename();

  void is_deleted(const ossia::net::node_base& n);

  t_symbol* x_tags;
  t_symbol* x_description;
};
}
} // namespace
