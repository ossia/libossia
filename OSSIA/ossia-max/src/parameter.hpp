#pragma once

#include <ossia-max/src/parameter_base.hpp>

namespace ossia
{

namespace max
{

class parameter : public parameter_base
{
public:
  using is_model = std::true_type;

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  static ossia::safe_vector<parameter*>& quarantine();

  // attribute setting method

  void set_mute();
  void set_unit();
  void set_rate();

  static void get_mute(parameter*x);
  static void get_rate(parameter*x);
  static void get_unit(parameter*x);

  static void update_attribute(parameter* x, ossia::string_view attribute);

  // attributes
  float m_rate;
  long m_mute;

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::parameter*);

  static void assist(
      ossia::max::parameter*, void*, long, long, char*);
  static t_max_err notify(ossia::max::parameter *x,
      t_symbol *s, t_symbol *msg, void *sender, void *data);

};

} // max namespace
} // ossia namespace
