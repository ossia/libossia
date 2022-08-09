#pragma once
#include <ossia-pd/src/node_base.hpp>

namespace ossia::pd
{

class model : public node_base
{
public:
  using is_model = std::true_type;
  model();

  bool register_node(const std::vector<t_matcher>& node);
  bool do_registration(const std::vector<t_matcher>& node);
  bool unregister();
  void register_children();
  void save_children_state();

  static void destroy(model* x);
  static void* create(t_symbol* name, int argc, t_atom* argv);
  static t_pd_err notify(model* x, t_symbol* s, t_symbol* msg, void* sender, void* data);

  static ossia::safe_set<model*>& quarantine();
};
} // namespace
