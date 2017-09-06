#pragma once
#include <ossia-max/src/object_base.hpp>

namespace ossia
{
namespace max
{

class parameter_base : public object_base
{
public:
  static void update_attribute(parameter_base* x, ossia::string_view attribute);

  parameter_base();

  // attribute setting method
  void set_access_mode();
  void set_repetition_filter();
  void set_enable();
  void set_minmax();
  void set_range();
  void set_bounding_mode();
  void set_default();
  void set_type();

  static void class_setup(t_class*c);

  static void get_range(parameter_base* x);
  static void get_min(parameter_base* x);
  static void get_max(parameter_base* x);
  static void get_bounding_mode(parameter_base* x);
  static void get_default(parameter_base* x);
  static void get_type(parameter_base* x);
  static void get_priority(parameter_base* x);
  static void get_access_mode(parameter_base* x);
  static void get_repetition_filter(parameter_base* x);
  static void get_enable(parameter_base* x);

  // attributes
  t_symbol* m_type{};
  t_atom m_default[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_atom m_min[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_atom m_max[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_atom m_range[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_symbol* m_bounding_mode{};
  t_symbol* m_access_mode{};
  float m_repetition_filter{};
  t_symbol* m_unit{};
  ossia::optional<ossia::unit_t> m_ounit;

  // size of size-variable attribute
  long m_default_size{};
  long m_range_size{};
  long m_min_size{};
  long m_max_size{};

  void* m_poll_clock{};

  /**
   * @brief t_obj_base::push : push a value to a node
   * @param x : caller that holds the node to push to
   * @param argc : number of value in the list
   * @param argv :  list of t_atom value(s)
   */
  static void push(parameter_base* x, t_symbol*, int argc, t_atom* argv);
  /**
   * @brief t_obj_base::bang send out the current value of the parameter
   * @param x
   */
  static void bang(parameter_base* x);
  static void output_value(parameter_base* x);

  static void set(parameter_base* x, t_symbol* s, int argc, t_atom* argv);
  static void in_float(parameter_base* x, double f);
  static void in_int(parameter_base* x, long int f);
  static void in_symbol(parameter_base* x, t_symbol* f);
};


} // namespace max
} // namespace ossia
