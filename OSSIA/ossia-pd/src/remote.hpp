#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_remote : t_object_base
{
  using is_view = std::true_type;
  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  ossia::net::device_base* m_dev{};
  t_symbol* m_unit;

  void set_unit();
  void set_mute();
  void set_enable();

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);

  static ossia::safe_vector<t_remote*>& quarantine()
  {
    static ossia::safe_vector<t_remote*> quarantine;
    return quarantine;
  }
};
} // namespace pd
} // namespace ossia
