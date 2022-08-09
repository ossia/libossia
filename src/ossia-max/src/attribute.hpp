#pragma once

#include <ossia/detail/optional.hpp>

#include <ossia-max/src/parameter_base.hpp>

namespace ossia
{
namespace max_binding
{

class device_base;
class attribute : public parameter_base
{
public:
  using is_attribute = std::true_type;

  void do_registration();
  void unregister();

  static void assist(attribute*, void*, long, long, char*);
  static void destroy(attribute* x);
  static void* create(t_symbol* name, int argc, t_atom* argv);

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  void on_device_deleted(const ossia::net::node_base&);

  void on_device_removing(device_base* obj);
  void on_device_created(device_base* obj);
  void on_node_renamed_callback(ossia::net::node_base& node, const std::string&);
};
} // namespace pd
} // namespace ossia
