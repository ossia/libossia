#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"
#include <boost/optional.hpp>

namespace ossia
{
namespace pd
{

struct t_remote;

struct t_matcher
{
  ossia::net::node_base* node;
  t_remote* parent;

  ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
      callbackit = ossia::none;

  t_matcher(ossia::net::node_base* n, t_remote* p); // constructor
  ~t_matcher();
  t_matcher(const t_matcher&) = delete;
  t_matcher(t_matcher&& other)
  {
      node = other.node;
      other.node = nullptr;
      
      parent = other.parent;
      other.parent = nullptr;
      
      callbackit = other.callbackit;
      other.callbackit = ossia::none;
  }
  t_matcher& operator=(const t_matcher&) = delete;
  t_matcher& operator=(t_matcher&& other)
  {
      node = other.node;
      other.node = nullptr;
      
      parent = other.parent;
      other.parent = nullptr;
      
      callbackit = other.callbackit;
      other.callbackit = ossia::none;
      
      return *this;
  }

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
