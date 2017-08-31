#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_view : public t_object_base
{
public:
  t_view();

  using is_view = std::true_type;

  bool register_node(std::vector<ossia::net::node_base*> node);
  bool do_registration(std::vector<ossia::net::node_base*> node);
  bool unregister();

  static ossia::safe_vector<t_view*>& quarantine();
};
}
} // namespace
