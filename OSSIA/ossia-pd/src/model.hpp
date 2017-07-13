#pragma once

#include "device.hpp"
#include "ossia-pd.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_model : t_obj_base
{
  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  static ossia::safe_vector<t_model*>& quarantine();
  t_symbol* x_tags;
  t_symbol* x_description;

  void isDeleted(const ossia::net::node_base& n);

  static ossia::safe_vector<t_model*>& rename();

  bool isRenamed(t_model* x);
  void renaming(t_model* x);
  void derenaming(t_model* x);
};
}
} // namespace
