#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"
#include <boost/optional.hpp>
#include "obj_pattern.hpp"

namespace ossia
{
namespace pd
{

struct t_matcher
{
  ossia::net::node_base* node;
  t_remote* parent;

  boost::optional<ossia::callback_container<ossia::value_callback>::iterator>
      callbackit = boost::none;

  t_matcher(ossia::net::node_base* n, t_remote* p); // constructor
  ~t_matcher();

  void set_value(const ossia::value& v);
};

struct t_remote : t_obj_base
{
  using is_view = std::true_type;
  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  boost::optional<ossia::callback_container<ossia::value_callback>::iterator>
      x_callbackit{};

  std::vector<boost::optional<ossia::callback_container<ossia::value_callback>::iterator> >
      x_callbackits;

  std::vector<t_matcher> x_matchers{};

  void is_deleted(const ossia::net::node_base& n);
  void on_address_created_callback(const ossia::net::address_base& addr);

  static ossia::safe_vector<t_remote*>& quarantine()
  {
    static ossia::safe_vector<t_remote*> quarantine;
    return quarantine;
  }
};
}
} // namespace
