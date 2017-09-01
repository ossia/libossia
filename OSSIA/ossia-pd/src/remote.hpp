#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

class t_remote : public t_object_base
{
public:
  using is_view = std::true_type;

  t_remote();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  ossia::net::device_base* m_dev{};
  t_symbol* m_unit;

  void set_unit();
  void set_mute();
  void set_enable();

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);

  static ossia::safe_vector<t_remote*>& quarantine();
};
} // namespace pd
} // namespace ossia
