#pragma once

#include <ossia-max/src/parameter_base.hpp>

namespace ossia
{

namespace max
{

class parameter : public parameter_base
{
public:
  using is_parameter = std::true_type;

  bool register_node(const std::vector<std::shared_ptr<t_matcher>>& node);
  bool do_registration(const std::vector<std::shared_ptr<t_matcher>>& node);
  bool unregister();

  static ossia::safe_set<parameter*>& quarantine();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::parameter*);

  static void assist(
      ossia::max::parameter*, void*, long, long, char*);
  static t_max_err notify(ossia::max::parameter *x,
      t_symbol *s, t_symbol *msg, void *sender, void *data);
  static void loadbang(parameter* x);

};

} // max namespace
} // ossia namespace
