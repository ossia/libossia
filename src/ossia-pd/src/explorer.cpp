// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "explorer.hpp"

#include "ossia-pd.hpp"

namespace ossia::pd
{
explorer::explorer(int argc, t_atom* argv)
    : object_base{ossia_pd::explorer_class}
{
}

void* explorer::create(t_symbol* s, int argc, t_atom* argv)
{
  return new explorer(argc, argv);
}

void explorer::destroy(explorer* x)
{
  if(x)
  {
    x->~explorer();
  }
}

void explorer::explore(ossia::pd::explorer* x, t_symbol* s, int argc, t_atom* argv)
{
  if(argc != 1 && argv->a_type != A_SYMBOL)
  {
    pd_error(x, "explore message requires a node name as a symbol argument");
    return;
  }
}

void explorer::search(ossia::pd::explorer* x, t_symbol* s, int argc, t_atom* argv)
{
  if(argc != 1 && argv->a_type != A_SYMBOL)
  {
    pd_error(x, "explore message requires a node name as a symbol argument");
    return;
  }
}

void explorer::monitor(ossia::pd::explorer* x, t_symbol* s, int argc, t_atom* argv) { }

void explorer::stop_monitoring() { }

t_max_err
explorer::notify(explorer* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  return 0;
}

extern "C" void setup_ossia0x2eexplorer(void)
{
  // instantiate the ossia.explorer class
  t_eclass* c = eclass_new(
      "ossia.explorer", (method)explorer::create, (method)explorer::destroy,
      (short)sizeof(explorer), CLASS_DEFAULT, A_GIMME, 0);

  class_addcreator((t_newmethod)explorer::create, gensym("ø.explorer"), A_GIMME, 0);

  eclass_addmethod(c, (method)explorer::explore, "explore", A_GIMME, 0);
  eclass_addmethod(c, (method)explorer::explore, "search", A_GIMME, 0);
  eclass_addmethod(c, (method)explorer::explore, "monitor", A_GIMME, 0);

#ifndef PURR_DATA
  eclass_register(CLASS_OBJ, c);
#endif
  ossia_pd::explorer_class = c;
}

}
