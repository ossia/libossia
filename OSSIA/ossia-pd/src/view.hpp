#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_view : t_object_base
{
  using is_view = std::true_type;

  bool register_node(std::vector<ossia::net::node_base*> node);
  bool do_registration(std::vector<ossia::net::node_base*> node);
  bool unregister();

  static ossia::safe_vector<t_view*>& quarantine();
};
}
} // namespace
