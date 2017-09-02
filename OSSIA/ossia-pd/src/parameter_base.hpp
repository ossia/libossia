#pragma once
#include <ossia-pd/src/ossia_obj_base.hpp>

namespace ossia
{
namespace pd
{

class parameter_base : public t_object_base
{
public:

  parameter_base(t_eclass*x);

  static void update_attribute(parameter_base* x, ossia::string_view attribute);

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
  void set_rate();

  static void get_range(parameter_base* x);
  static void get_min(parameter_base* x);
  static void get_max(parameter_base* x);
  static void get_bounding_mode(parameter_base* x);
  static void get_default(parameter_base* x);
  static void get_unit(parameter_base* x);
  static void get_type(parameter_base* x);
  static void get_hidden(parameter_base* x);
  static void get_priority(parameter_base* x);
  static void get_access_mode(parameter_base* x);
  static void get_repetition_filter(parameter_base* x);
  static void get_tags(parameter_base* x);
  static void get_description(parameter_base* x);
  static void get_enable(parameter_base* x);

  // attributes
  t_symbol* m_type{};
  t_atom m_default[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_min[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_max[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_range[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_symbol* m_bounding_mode{};
  t_symbol* m_access_mode{};
  t_float m_repetition_filter{};
  t_symbol* m_unit{};
  t_atom m_tags[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_description[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  int m_priority{};
  bool m_hidden{};

  // size of size-variable attribute
  long m_default_size{};
  long m_range_size{};
  long m_min_size{};
  long m_max_size{};
  long m_tags_size{};
  long m_description_size{};
};

} // namespace pd
} // namespace ossia
