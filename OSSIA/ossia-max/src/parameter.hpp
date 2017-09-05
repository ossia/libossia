#pragma once

#include <ossia-max/src/parameter_base.hpp>

namespace ossia
{

namespace max
{

#pragma mark -
#pragma mark t_parameter structure declaration

class parameter : public object_base
{
public:
  using is_model = std::true_type;

  parameter();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  static ossia::safe_vector<parameter*>& quarantine();

  // attribute setting method
  void set_access_mode();
  void set_repetition_filter();
  void set_description();
  void set_tags();
  void set_priority();
  void set_enable();
  void set_mute();
  void set_hidden();
  void set_minmax();
  void set_range();
  void set_bounding_mode();
  void set_default();
  void set_unit();
  void set_type();

  // attributes
  t_symbol* m_type;
  t_atom m_default[OSSIA_MAX_MAX_ATTR_SIZE];
  t_atom m_min[OSSIA_MAX_MAX_ATTR_SIZE];
  t_atom m_max[OSSIA_MAX_MAX_ATTR_SIZE];
  t_atom m_range[OSSIA_MAX_MAX_ATTR_SIZE];
  t_symbol* m_bounding_mode;
  t_symbol* m_access_mode;
  float m_repetition_filter = 0;
  t_symbol* m_unit;
  t_atom m_tags[OSSIA_MAX_MAX_ATTR_SIZE];
  t_symbol* m_description;
  int m_priority;
  bool m_hidden;

  // size of size-variable attribute
  long m_default_size;
  long m_range_size;
  long m_min_size;
  long m_max_size;
  long m_tags_size;

  static void push_default_value(parameter* x);

};

#pragma mark -
#pragma mark Utilities


} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_parameter class declaration

extern "C" {
void* ossia_parameter_new(t_symbol*, long, t_atom*);
void ossia_parameter_free(ossia::max::parameter*);

void ossia_parameter_assist(
    ossia::max::parameter*, void*, long, long, char*);
void ossia_parameter_in_float(ossia::max::parameter*, double f);
void ossia_parameter_in_int(ossia::max::parameter* x, long int f);
void ossia_parameter_in_bang(ossia::max::parameter*);
void ossia_parameter_in_symbol(ossia::max::parameter*, t_symbol*);
void ossia_parameter_in_char(ossia::max::parameter*, char);
void ossia_parameter_in_anything(
    ossia::max::parameter*, t_symbol*, long, t_atom*);
t_max_err ossia_parameter_notify(ossia::max::parameter *x,
    t_symbol *s, t_symbol *msg, void *sender, void *data);
}
