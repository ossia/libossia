#pragma once
#include <ossia-max/src/object_base.hpp>

namespace ossia
{
namespace max_binding
{

class node_base : public object_base
{
public:
  static void preset(node_base *x, t_symbol* s, long argc, t_atom* argv);
  static void class_setup(t_class* c);
  static void set(node_base* x, t_symbol* s, int argc, t_atom* argv);
  static void push_default_value(node_base* x);

  /**
   * @brief get_namespace send the namespace through dump outlet
   * @note only relevant for client, device, model and view objects.
   * @param x
   */
  static void get_namespace(node_base* x, t_symbol* s, long argc, t_atom* argv);

};

} // namespace max
} // namespace ossia
