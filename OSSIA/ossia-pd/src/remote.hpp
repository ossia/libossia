#pragma once
#include "device.hpp"
#include "ossia_obj_base.hpp"
#include <boost/optional.hpp>

namespace ossia
{
namespace pd
{

struct t_remote : t_obj_base
{
  using is_view = std::true_type;
  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  boost::optional<ossia::callback_container<ossia::value_callback>::iterator>
      x_callbackit;

  void isDeleted(const ossia::net::node_base& n)
  {
    if (!x_dead)
    {
      unregister();
    }
  }

  static ossia::safe_vector<t_remote*>& quarantine()
  {
    static ossia::safe_vector<t_remote*> quarantine;
    return quarantine;
  }
};
}
} // namespace
