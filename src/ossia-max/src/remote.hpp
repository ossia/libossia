#pragma once
#include <ossia-max/src/parameter_base.hpp>
#include <ossia/detail/optional.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_remote structure declaration

class remote : public parameter_base
{
public:
  using is_remote = std::true_type;

  bool register_node(const std::vector<std::shared_ptr<matcher>>& node, bool output_value = true);
  bool do_registration(const std::vector<std::shared_ptr<matcher>>& node, bool output_value = true);
  bool unregister();

  ossia::safe_set<ossia::net::generic_device*> m_devices{};

  void set_unit();

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  void on_device_deleted(const ossia::net::node_base&);

  static void update_attribute(remote* x, ossia::string_view attribute, const ossia::net::node_base* node);
  static t_max_err notify(remote*x, t_symbol*s, t_symbol* msg, void* sender, void* data);

  static void get_mess_cb(remote* x, t_symbol* s);
  static void get_unit(remote*x);
  static void get_mute(remote*x);

  static void assist(ossia::max::remote*, void*, long, long, char*);
  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::remote*);
};

} // max namespace
} // ossia namespace
