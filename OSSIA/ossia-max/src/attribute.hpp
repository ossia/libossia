#pragma once

#include <ossia-max/src/parameter_base.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/network/common/path.hpp>

namespace ossia
{
namespace max
{

class attribute : public parameter_base
{
public:
  using is_view = std::true_type;

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  ossia::net::device_base* m_dev{};
  float m_rate_min;

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  static void update_attribute(attribute* x, ossia::string_view attribute, const ossia::net::node_base* node);
  static void bind(attribute* x, t_symbol* address);

  static t_max_err notify(attribute*x, t_symbol*s, t_symbol* msg, void* sender, void* data);

  static void destroy(attribute* x);
  static void* create(t_symbol* name, int argc, t_atom* argv);

  static ossia::safe_set<attribute*>& quarantine();

  void on_device_deleted(const ossia::net::node_base&);
private:
  void update_path(ossia::string_view name);
  ossia::optional<ossia::traversal::path> m_path;

};
} // namespace pd
} // namespace ossia
