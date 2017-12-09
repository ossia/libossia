// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-pd/src/assert.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>
#include "ossia-pd.hpp"
#include <ossia/detail/thread.hpp>

namespace ossia
{
namespace pd
{

static t_eclass* assert_class;

void* assert::create(t_symbol* s, int argc, t_atom* argv)
{
  assert* x = (assert*)eobj_new(assert_class);

  t_binbuf* d = binbuf_via_atoms(argc, argv);

  x->m_name = gensym("");
  t_atom* a = x->m_atom;
  SETFLOAT(a,0);

  ebox_attrprocess_viabinbuf(x, d);

  argc = argc > 256 ? 256 : argc;
  x->m_size = atoms_get_attributes_offset(argc,argv);

  memcpy(a, argv, x->m_size*sizeof(t_atom));

  return x;
}

void assert::in_bang(ossia::pd::assert* x)
{
  // x->m_armed=true;
}

void assert::in_anything(ossia::pd::assert* x, t_symbol* s, int argc, t_atom* argv)
{
  t_atom* a = x->m_atom;

  if (argc != x->m_size)
    x->quit();

  while (argc--)
  {
    if ( a->a_type == argv->a_type )
    {
      switch(a->a_type)
      {
        case A_FLOAT:
          if (a->a_w.w_float != argv->a_w.w_float)
            x->quit();
          break;
        case A_SYMBOL:
          if (a->a_w.w_symbol != argv->a_w.w_symbol)
            x->quit();
          break;
        default:
          ;
      }
    } else {
      x->quit();
    }
    argv++;
    a++;
  }
}

void assert::quit()
{
  logpost(this, 1, "assert %s failed", m_name->s_name);
  if(m_armed)
    exit(1);
  else
    logpost(this, 0, "crash!!!");
}

void assert::destroy(assert* x)
{
}

extern "C" void setup_ossia0x2eassert(void)
{
  // instantiate the ossia.assert class

  t_eclass* c = eclass_new(
      "ossia.assert", (method)assert::create, (method)assert::destroy,
      (short)sizeof(assert), CLASS_DEFAULT, A_GIMME, 0);

  class_addcreator((t_newmethod)assert::create,gensym("ø.assert"), A_GIMME, 0);

  eclass_addmethod(
      c, (method)assert::in_anything,
      "anything", A_GIMME, 0);
  eclass_addmethod(
      c, (method)assert::in_bang,
      "bang", A_NULL, 0);
  eclass_addmethod(
      c, (method)assert::in_bang,
      "loadbang", A_NULL, 0);

  CLASS_ATTR_SYMBOL(c, "name", 0, assert, m_name);

  assert_class = c;
}

} // pd namespace
} // ossia namespace
