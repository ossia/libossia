#pragma once

#include <ossia-pd/src/parameter_base.hpp>
#include <ossia/detail/optional.hpp>

namespace ossia
{
namespace pd
{

class attribute : public parameter_base
{
public:
  using is_attribute = std::true_type;

  attribute();

  bool register_node(const std::vector<std::shared_ptr<t_matcher>>& node);
  bool do_registration(const std::vector<std::shared_ptr<t_matcher>>& node);
  bool unregister();

  ossia::net::device_base* m_dev{};

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  static void click( attribute* x, t_floatarg xpos, t_floatarg ypos,
                     t_floatarg shift, t_floatarg ctrl, t_floatarg alt);
  static t_pd_err notify(attribute*x, t_symbol*s, t_symbol* msg, void* sender, void* data);

  static void destroy(attribute* x);
  static void* create(t_symbol* name, int argc, t_atom* argv);

  static ossia::safe_set<attribute*>& quarantine();

  void on_device_deleted(const ossia::net::node_base&);
};
} // namespace pd
} // namespace ossia
