#pragma once

#include "device.hpp"
#include "parameter_base.hpp"

namespace ossia
{
namespace pd
{

class remote : public parameter_base
{
public:
  using is_view = std::true_type;

  remote();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  ossia::net::device_base* m_dev{};
  t_symbol* m_unit;
  float m_rate_min;

  void set_unit();
  void set_rate();

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  static void update_attribute(remote* x, ossia::string_view attribute);

  static ossia::safe_vector<remote*>& quarantine();
};
} // namespace pd
} // namespace ossia
