#pragma once
#include <ossia-pd/src/object_base.hpp>

namespace ossia
{
namespace pd
{

class parameter_base : public object_base
{
public:

  parameter_base(t_eclass*x);

  static void update_attribute(parameter_base* x, ossia::string_view attribute, const ossia::net::node_base* node = nullptr);

  // attribute setting method
  void set_access_mode();
  void set_repetition_filter();
  void set_enable();
  void set_mute();
  void set_minmax();
  void set_range();
  void set_bounding_mode();
  void set_default();
  void set_type();
  void set_rate();
  void set_unit();

  static void class_setup(t_eclass*c);

  static void get_mess_cb(parameter_base* x, t_symbol* s);
  static void get_unit(parameter_base*x, std::vector<t_matcher*> nodes);
  static void get_rate(parameter_base*x, std::vector<t_matcher*> nodes);
  static void get_mute(parameter_base*x, std::vector<t_matcher*> nodes);
  static void get_domain(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_min(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_max(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_bounding_mode(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_default(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_type(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_priority(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_access_mode(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_repetition_filter(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_enable(parameter_base* x, std::vector<t_matcher*> nodes);
  static void get_queue_length(parameter_base*x, std::vector<t_matcher*> nodes);

  // attributes
  t_symbol* m_type{};
  t_atom m_default[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_min[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_max[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_range[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_symbol* m_bounding_mode{};
  t_symbol* m_access_mode{};
  t_float m_repetitions{1};
  t_symbol* m_unit{};
  int m_mute{0};
  int m_enable{1};
  ossia::optional<ossia::unit_t> m_ounit;

  // size of size-variable attribute
  long m_default_size{};
  long m_range_size{};
  long m_min_size{};
  long m_max_size{};

  /**
   * @brief t_obj_base::push : push a value to a node
   * @param x : caller that holds the node to push to
   * @param argc : number of value in the list
   * @param argv :  list of t_atom value(s)
   */
  static void push(parameter_base* x, t_symbol*, int argc, t_atom* argv);

  static void push_default_value(parameter_base* x);
  /**
   * @brief t_obj_base::bang send out the current value of the parameter
   * @param x
   */
  static void bang(parameter_base* x);
  static void output_value(parameter_base* x);
};


} // namespace pd
} // namespace ossia
