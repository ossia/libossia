#pragma once

#include <ossia-pd/src/device.hpp>
#include <ossia-pd/src/parameter_base.hpp>

namespace ossia
{
namespace pd
{

struct view : public node_base
{
public:
  view();

  using is_view = std::true_type;

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  static ossia::safe_vector<view*>& quarantine();
};
}
} // namespace
