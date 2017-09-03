#pragma once

#include <ossia-pd/src/device.hpp>
#include <ossia-pd/src/parameter_base.hpp>

namespace ossia
{
namespace pd
{

class model : public node_base
{
public:
  using is_model = std::true_type;
  model();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();
  void register_children();

  static ossia::safe_vector<model*>& quarantine();
};
}
} // namespace
