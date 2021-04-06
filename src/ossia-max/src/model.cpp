// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "utils.hpp"

#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/node_functions.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_model class methods

extern "C" void ossia_model_setup()
{
  auto& ossia_library = ossia_max::instance();

  t_class* c = class_new(
      "ossia.model", (method)model::create, (method)model::destroy,
      (long)sizeof(ossia::max::model), 0L, A_GIMME, 0);

  node_base::class_setup(c);

  class_addmethod(
      c, (method)model::assist,
        "assist", A_CANT, 0);

  class_addmethod(c, (method) address_mess_cb<model>, "address",   A_SYM, 0);
  class_addmethod(c, (method) model::get_mess_cb, "get",   A_SYM, 0);

  class_addmethod(
        c, (method)model::notify,
        "notify", A_CANT, 0);

  class_register(CLASS_BOX, c);
  ossia_library.ossia_model_class = c;
}

namespace ossia
{
namespace max
{

void* model::create(t_symbol*, long argc, t_atom* argv)
{
  auto x = make_ossia<model>();

  if (x)
  {
    critical_enter(0);
    auto& pat_desc = ossia_max::get_patcher_descriptor(x->m_patcher);
    if( !pat_desc.model && !pat_desc.view)
    {
      pat_desc.model = x;
    }
    else
    {
      error("You can put only one [ossia.model] or [ossia.view] per patcher");
      object_free(x);
      critical_exit(0);
      return nullptr;
    }

    x->m_otype = object_class::model;

    // make outlets
    x->m_dumpout = outlet_new(x, NULL); // anything outlet to dump model state

    x->m_description = _sym_nothing;
    x->m_tags_size = 0;

    // process attr args, if any
    long attrstart = attr_args_offset(argc, argv);
    attr_args_process(x, argc - attrstart, argv + attrstart);

    // check name argument
    x->m_name = _sym_nothing;
    if (argc > 0 && attrstart > 0 )
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }
    }

    defer_low(x, (method) object_base::loadbang, nullptr, 0, nullptr);

    ossia_max::instance().models.push_back(x);
    critical_exit(0);
  }

  return (x);
}

void model::destroy(model* x)
{
  critical_enter(0);
  x->m_dead = true;
  x->save_children_state();
  x->m_node_selection.clear();
  x->m_matchers.clear();
  x->m_registered = false;

  register_children_in_patcher_recursively(x->m_patcher, x->find_parent_object());
  output_all_values(x->m_patcher, true);

  ossia_max::instance().models.remove_all(x);
  if(x->m_dumpout)
    outlet_delete(x->m_dumpout);
  x->~model();
  critical_exit(0);
}

void model::assist(model*, void*, long m, long, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Model input");
  }
  else
  {
    sprintf(s, "Dumpout");
  }
}

void model::do_registration()
{
  ossia::string_view name(m_name->s_name);

  m_registered = true;

  m_matchers = find_or_create_matchers();

  m_selection_path.reset();
  fill_selection();

  set_priority();
  set_description();
  set_tags();
  set_hidden();
  set_recall_safe();
}

} // max namespace
} // ossia namespace
