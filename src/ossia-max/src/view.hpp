#pragma once
#include <ossia-max/src/node_base.hpp>

namespace ossia
{
namespace max
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

  void on_device_created(ossia::max::device_base* device);
  void on_device_removing(ossia::max::device_base* device);

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::view*);
};

} // max namespace
} // ossia namespace


