#pragma once
#include <ossia-pd/src/object_base.hpp>

namespace ossia
{
namespace pd
{
class explorer : public object_base // inherite from object_base just to use default constructor
{
public:
  explorer(int argc, t_atom* argv);

  static void* create(t_symbol* s, int argc, t_atom* argv);
  static void  destroy(explorer* x);

  static t_pd_err notify(explorer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

};
} // pd namespace
} // ossia namespace
