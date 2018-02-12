// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "explorer.hpp"
#include "ossia-pd.hpp"

namespace ossia {
namespace pd {
explorer::explorer(int argc, t_atom* argv) :
  object_base{ossia_pd::explorer_class}
{

}

void* explorer::create(t_symbol* s, int argc, t_atom* argv)
{
  return new explorer(argc, argv);
}

void explorer::destroy(explorer* x)
{
  if (x)
  {
    x->~explorer();
  }
}

extern "C" void setup_ossia0x2eexplorer(void)
{
  // instantiate the ossia.logger class

  t_eclass* c = eclass_new(
      "ossia.explorer", (method)explorer::create, (method)explorer::destroy,
      (short)sizeof(explorer), CLASS_DEFAULT, A_GIMME, 0);

  class_addcreator((t_newmethod)explorer::create,gensym("ø.explorer"), A_GIMME, 0);

  /*
  eclass_addmethod(
      c, (method)explorer::in_anything,
      "anything", A_GIMME, 0);
  */

  eclass_register(CLASS_OBJ, c);
  ossia_pd::explorer_class = c;
}

}
}
