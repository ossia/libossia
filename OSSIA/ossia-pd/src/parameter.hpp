#pragma once

#include <ossia-pd/src/parameter_base.hpp>

namespace ossia
{
namespace pd
{

class parameter : public parameter_base
{
public:
  using is_parameter = std::true_type;

  parameter();

  bool register_node(const std::vector<t_matcher>& node);
  bool do_registration(const std::vector<t_matcher>& node);
  bool unregister();

  static t_pd_err notify(
      parameter*x, t_symbol*s, t_symbol* msg, void* sender, void* data);
  static void update_attribute(
      parameter* x, string_view attribute, const net::node_base* node);

  static void* create(t_symbol* name, int argc, t_atom* argv);
  static void destroy(parameter* x);


};
} // namespace pd
} // namespace ossia
