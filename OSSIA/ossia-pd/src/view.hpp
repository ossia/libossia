#pragma once

#include "device.hpp"
#include "ossia-pd.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_view : t_obj_base
{
  using is_view = std::true_type;

  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  void is_deleted(const ossia::net::node_base& n)
  {
    if (!x_dead)
    {
      unregister();
    }
  }

  static ossia::safe_vector<t_view*>& quarantine()
  {
    static ossia::safe_vector<t_view*> quarantine;
    return quarantine;
  }
};
}
} // namespace
