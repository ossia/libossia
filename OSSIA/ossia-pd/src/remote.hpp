#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"
#include <boost/optional.hpp>

namespace ossia
{
namespace pd
{

struct t_remote;

class t_matcher
{
public:
  t_matcher(ossia::net::node_base* n, t_remote* p); // constructor
  ~t_matcher();
  t_matcher(const t_matcher&) = delete;
  t_matcher(t_matcher&& other);
  t_matcher& operator=(const t_matcher&) = delete;
  t_matcher& operator=(t_matcher&& other);
  
  void set_value(const ossia::value& v);
  auto get_node() const { return node; }
  
  inline bool operator==(const t_matcher& rhs)
  { return (get_node() == rhs.node); }

  
  private:
    ossia::net::node_base* node{};
    t_remote* parent{};

    ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
        callbackit = ossia::none;

};

struct t_remote : t_obj_base
{
  using is_view = std::true_type;
  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
      x_callbackit{};

  std::vector<boost::optional<ossia::callback_container<ossia::value_callback>::iterator> >
      x_callbackits;

  std::vector<t_matcher> x_matchers{};

  bool x_is_pattern{};
  ossia::net::device_base* x_dev{};

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
