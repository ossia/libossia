#pragma once

#include <ossia-pd/src/parameter_base.hpp>

namespace ossia
{
namespace pd
{

class parameter : public parameter_base
{
public:
  using is_model = std::true_type;

  parameter();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  static ossia::safe_vector<parameter*>& quarantine();

  static t_pd_err notify(parameter*x, t_symbol*s, t_symbol* msg, void* sender, void* data);
  static void get_unit(parameter*x);
  static void get_mute(parameter*x);
  static void get_rate(parameter*x);

  static void update_attribute(parameter* x, ossia::string_view attribute);

  void set_unit();

  static void* create(t_symbol* name, int argc, t_atom* argv);
  static void destroy(parameter* x);


};
} // namespace pd
} // namespace ossia
