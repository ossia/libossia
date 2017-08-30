#pragma once

#include "device.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_param : t_object_base
{
  using is_model = std::true_type;
  bool register_node(std::vector<ossia::net::node_base*> node);
  bool do_registration(std::vector<ossia::net::node_base*> node);
  bool unregister();

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
  t_atom m_default[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom m_min[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom m_max[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom m_range[OSSIA_PD_MAX_ATTR_SIZE];
  t_symbol* m_bounding_mode;
  t_symbol* m_access_mode;
  t_float m_repetition_filter = 0;
  t_symbol* m_unit;
  t_atom m_tags[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom m_description[OSSIA_PD_MAX_ATTR_SIZE];
  int m_priority;
  bool m_hidden;

  // size of size-variable attribute
  long m_default_size;
  long m_range_size;
  long m_min_size;
  long m_max_size;
  long m_tags_size;
  long m_description_size;

  static ossia::safe_vector<t_param*>& quarantine();

};
}
} // namespace
