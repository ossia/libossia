#pragma once
#include <ossia-max/src/object_base.hpp>

namespace ossia
{
namespace max
{

class node_base : public object_base
{
public:
  static void preset(node_base *x, t_symbol* s, long argc, t_atom* argv);
  static void declare_attributes(t_class* c);

  /**
   * @brief get_namespace send the namespace through dump outlet
   * @note only relevant for client, device, model and view objects.
   * @param x
   */
  static void get_namespace(node_base* x);

};

} // namespace max
} // namespace ossia
