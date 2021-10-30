#pragma once
#include <ossia-max/src/object_base.hpp>

namespace ossia
{
namespace max_binding
{

class parameter_base : public object_base
{
public:
  static void update_attribute(parameter_base* x, ossia::string_view attribute, const ossia::net::node_base* node);
  static t_max_err notify(parameter_base *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

  parameter_base();

  void output_values(bool only_default);

  // attribute setting method
  void set_access_mode();
  void set_repetition_filter();
  void set_critical();
  void set_enable();
  void set_minmax();
  void set_range();
  void set_bounding_mode();
  void set_default();
  void set_type();
  void set_rate();
  void set_unit();
  void set_mute();

  static void class_setup(t_class*c);

  static void get_mess_cb(parameter_base* x, t_symbol* s);
  static void get_unit(parameter_base*x, std::vector<matcher*> nodes);
  static void get_rate(parameter_base*x, std::vector<matcher*> nodes);
  static void get_mute(parameter_base*x, std::vector<matcher*> nodes);
  static void get_domain(parameter_base* x, std::vector<matcher*> nodes);
  static void get_min(parameter_base* x, std::vector<matcher*> nodes);
  static void get_max(parameter_base* x, std::vector<matcher*> nodes);
  static void get_bounding_mode(parameter_base* x, std::vector<matcher*> nodes);
  static void get_default(parameter_base* x, std::vector<matcher*> nodes);
  static void get_type(parameter_base* x, std::vector<matcher*> nodes);
  static void get_access_mode(parameter_base* x, std::vector<matcher*> nodes);
  static void get_repetition_filter(parameter_base* x, std::vector<matcher*> nodes);
  static void get_critical(parameter_base* x, std::vector<matcher*> nodes);
  static void get_enable(parameter_base* x, std::vector<matcher*> nodes);

  // attributes
  t_symbol* m_type{};
  t_atom m_default[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_atom m_min[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_atom m_max[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_atom m_range[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_symbol* m_bounding_mode{};
  t_symbol* m_access_mode{};
  long m_repetitions{1};
  long m_mute{0};
  long m_enable{1};
  long m_critical{0};
  t_symbol* m_unit{};
  std::optional<ossia::unit_t> m_local_unit;

  // size of size-variable attribute
  long m_default_size{};
  long m_range_size{};
  long m_min_size{};
  long m_max_size{};

  bool m_set_flag{};
  bool m_inlet_locked{};

  /**
   * @brief t_obj_base::push : push a value to a node
   * @param x : caller that holds the node to push to
   * @param argc : number of value in the list
   * @param argv :  list of t_atom value(s)
   */
  static void push(parameter_base* x, t_symbol*, int argc, t_atom* argv);

  /**
   * @brief Send a list of key-value pairs
   */
  static void push_one(parameter_base* x, t_symbol* s, int argc, t_atom* argv);

  /**
   * @brief t_obj_base::bang send out the current value of the parameter
   * @param x
   */
  static void bang(parameter_base* x);
  static void push_default_value(parameter_base* x);

  static void set(parameter_base* x, t_symbol* s, int argc, t_atom* argv);
  static void in_float(parameter_base* x, double f);
  static void in_int(parameter_base* x, long int f);
  static void in_symbol(parameter_base* x, t_symbol* f);
};


} // namespace max
} // namespace ossia
