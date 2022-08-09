#pragma once
#include <ossia-max/src/node_base.hpp>

namespace ossia
{
namespace max_binding
{

class device_base;

class view : public node_base
{
public:
  using is_view = std::true_type;

  void do_registration();
  void unregister();

  ossia::safe_set<ossia::net::device_base*> m_devices{};

  void on_node_created_callback(ossia::net::node_base& node);
  void on_node_renamed_callback(ossia::net::node_base& node, const std::string&);
  void on_device_created(ossia::max_binding::device_base* device);
  void on_device_removing(ossia::max_binding::device_base* device);

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max_binding::view*);
};

} // max namespace
} // ossia namespace
