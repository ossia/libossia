#pragma once
#include <ossia-pd/src/node_base.hpp>

namespace ossia
{
namespace pd
{

class model : public node_base
{
public:
  using is_model = std::true_type;
  model();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();
  void register_children();

  static ossia::safe_vector<model*>& quarantine();

  static void destroy(model* x);
  static void* create(t_symbol* name, int argc, t_atom* argv);
  static t_pd_err notify(model*x, t_symbol*s, t_symbol* msg, void* sender, void* data);

};
}
} // namespace
