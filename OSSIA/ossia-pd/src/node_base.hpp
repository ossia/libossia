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
};

} // namespace pd
} // namespace ossia
