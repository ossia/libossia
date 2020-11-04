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

  // instantiate the ossia.parameter class
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

void* model::create(t_symbol* name, long argc, t_atom* argv)
{
  auto x = make_ossia<model>();

  if (x)
  {
    auto patcher = x->m_patcher;
    if( ossia_max::instance().patchers[patcher].model
     && ossia_max::instance().patchers[patcher].view)
    {
      ossia_max::instance().patchers[patcher].model = x;
    }
    else
    {
      error("You can put only one [ossia.model] or [ossia.view] per patcher");
      object_free(x);
      x->~model();
      return nullptr;
    }

    x->m_otype = object_class::model;

    // make outlets
    x->m_dumpout = outlet_new(x, NULL); // anything outlet to dump model state

    x->m_description = _sym_nothing;
    x->m_tags_size = 0;

    // check name argument
    x->m_name = _sym_nothing;
    if (argc > 0 && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }
    }

    if (x->m_name == _sym_nothing)
    {
      object_error((t_object*)x, "needs a name as first argument");
      x->m_name = gensym("untitledModel");
      return x;
    }

    // process attr args, if any
    long attrstart = attr_args_offset(argc, argv);
    attr_args_process(x, argc - attrstart, argv + attrstart);

    // need to schedule a loadbang because objects only receive a loadbang when patcher loads.
    x->m_reg_clock = clock_new(x, (method) object_base::loadbang);
    clock_set(x->m_reg_clock, 1);

    ossia_max::instance().models.push_back(x);
  }

  return (x);
}

void model::destroy(model* x)
{
  auto pat_it = ossia_max::instance().patchers.find(x->m_patcher);
  if(pat_it != ossia_max::instance().patchers.end())
  {
    auto& pat_desc = pat_it->second;
    pat_desc.model = nullptr;
    if(pat_desc.empty())
    {
      ossia_max::instance().patchers.erase(pat_it);
    }
  }

  x->m_dead = true;
  x->unregister();
  ossia_max::instance().models.remove_all(x);
  outlet_delete(x->m_dumpout);
  x->~model();
}

void model::assist(model* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "Model input", a);
  }
  else
  {
    sprintf(s, "Dumpout", a);
  }
}

bool model::do_registration(const std::vector<std::shared_ptr<matcher>>& matchers)
{
  ossia::string_view name(m_name->s_name);

  m_registered = true;

  // FIXME in case of address with pattern, we shouldn't clear m_matchers here
  // instead we should rely on device node_deleting signal to delete relevant matchers
  m_matchers.clear();
  m_matchers.reserve(matchers.size());

  for (auto& m : matchers)
  {
    auto node = m->get_node();
    if (m_addr_scope == ossia::net::address_scope::absolute)
    {
      // get root node
      node = &node->get_device().get_root_node();
      // and remove starting '/'
      name = name.substr(1);

      auto pos = name.rfind('/');
      auto parent_address = name.substr(0, pos);
    }
    m_parent_node = node;

    if (node->find_child(name))
    {
      // TODO : check if node has a parameter
      // in that case it is an ø.param, with no doubts
      // then remove it (and associated ø.param
      // and ø.remote will be unregistered automatically)

      // we have to check if a node with the same name already exists to avoid
      // auto-incrementing name
      std::vector<object_base*> obj = find_children_to_register(
            &m_object, m_patcher, gensym("ossia.model"));
      for (auto v : obj)
      {
        if (v->m_otype == object_class::param)
        {
          parameter* param = (parameter*)v;
          if (ossia::string_view(param->m_name->s_name) == name)
          {
            // if we already have a parameter of that
            // name, unregister it
            // we will register it again after node creation
            param->unregister();
            continue;
          }
        }
      }
    }

    auto m_nodes = ossia::net::create_nodes(*node, name);
    for (auto n : m_nodes)
    {
      m_matchers.emplace_back(std::make_shared<matcher>(n, this));
    }

    fill_selection();

    set_priority();
    set_description();
    set_tags();
    set_hidden();
    set_recall_safe();
  }

  return (!m_matchers.empty() || m_is_pattern);
}

bool model::unregister()
{
  save_children_state();
  m_matchers.clear();

  auto nodes = find_parent_nodes();
  auto patcher = m_patcher;
  register_children_in_patcher_recursively(patcher, this, nodes);

  m_registered = false;

  return true;
}

void model::save_children_state()
{
  for(auto& m : m_matchers)
  {
    // FIXME : why iterating over all parameters here ?
    // shouldn't it be fine to get matcher's owner and cast it according to it's m_otype ?
    for(auto x : ossia_max::instance().parameters.reference() )
    {
      if(x->m_parent_node == m->get_node())
      {
        x->save_values();
      }
    }

    for(auto x : ossia_max::instance().models.reference() )
    {
      if(x->m_parent_node == m->get_node())
      {
        x->save_children_state();
      }
    }
  }
}

} // max namespace
} // ossia namespace
