#pragma once

#include <ossia-pd/src/object_base.hpp>

namespace ossia
{
namespace pd
{

class node_base : public object_base
{
public:
  node_base(t_eclass* x);

  static void preset(object_base *x, t_symbol* s, int argc, t_atom* argv);
  static void class_setup(t_eclass* c);

  /**
   * @brief obj_namespace send the namespace through dump outlet
   * @note only relevant for client, device, model and view objects.
   * @param x
   */
  static void get_namespace(object_base* x);

  static void update_attribute(object_base* x, ossia::string_view attribute);

};

} // namespace pd
} // namespace ossia
