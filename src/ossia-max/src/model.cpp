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
    auto patcher = get_patcher(&x->m_object);
    if( ossia_max::instance().patchers[patcher].models.empty()
     && ossia_max::instance().patchers[patcher].views.empty())
    {
      ossia_max::instance().patchers[patcher].models.push_back(x);
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

    if(ossia_max::instance().patchers[patcher].loadbanged)
    {
      auto matchers = x->find_parent_nodes();
      x->do_registration(matchers);
    }

    ossia_max::instance().models.push_back(x);
  }

  return (x);
}

void model::destroy(model* x)
{
  auto pat_it = ossia_max::instance().patchers.find(get_patcher(&x->m_object));
  if(pat_it != ossia_max::instance().patchers.end())
  {
    auto& pat_desc = pat_it->second;
    pat_desc.models.remove_all(x);
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

bool model::register_node(const std::vector<std::shared_ptr<t_matcher>>& matchers)
{
  bool res = do_registration(matchers);

  if (res)
  {
    ossia_max::instance().nr_models.remove_all(this);
    register_children();
  }
  else
  {
    ossia_max::instance().nr_models.push_back(this);
  }

  return res;
}

bool model::do_registration(const std::vector<std::shared_ptr<t_matcher>>& matchers)
{
  unregister();

  ossia::string_view name(m_name->s_name);

  for (auto& m : matchers)
  {
    auto node = m->get_node();
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
            &m_object, get_patcher(&m_object), gensym("ossia.model"));
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
      m_matchers.emplace_back(std::make_shared<t_matcher>(n, this));
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

void model::register_children()
{
  std::vector<object_base*> children = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.model"));

  // TODO take care of not registering a child twice

  auto& nr_models = ossia_max::instance().nr_models;
  auto& nr_parameters = ossia_max::instance().nr_parameters;
  auto& nr_remotes = ossia_max::instance().nr_remotes;

  for(auto child : children)
  {
    if (child->m_otype == object_class::model)
    {
      nr_models.push_back(static_cast<model*>(child));
    }
    else if (child->m_otype == object_class::param)
    {
      nr_parameters.push_back(static_cast<parameter*>(child));
    }
    else if (child->m_otype == object_class::remote)
    {
      nr_remotes.push_back(static_cast<remote*>(child));
    }
  }

  auto copy = children;
  for (auto child : copy)
  {
    if (child->m_otype == object_class::model && ossia::contains(nr_models.reference(), child))
    {
      ossia::max::model* model = (ossia::max::model*)child;

      // ignore itself
      if (model == this)
        continue;

      if(model->m_addr_scope == ossia::net::address_scope::relative && !m_matchers.empty())
        model->register_node(m_matchers);
      else
        ossia_register(model);

      ossia::remove_erase(children, child);
    }
  }

  copy = children;
  for (auto child : copy)
  {
    if (child->m_otype == object_class::param && ossia::contains(nr_parameters.reference(), child))
    {
      ossia::max::parameter* parameter = (ossia::max::parameter*)child;
      if(parameter->m_addr_scope == ossia::net::address_scope::relative && !m_matchers.empty())
        parameter->register_node(m_matchers);
      else // FIXME is the else statement needed ?
        ossia_register(parameter);

      ossia::remove_erase(children, child);
    }
  }

  copy = children;
  for (auto child : copy)
  {
    if (child->m_otype == object_class::remote && ossia::contains(nr_remotes.reference(), child))
    {
      ossia::max::remote* remote = (ossia::max::remote*)child;

      if(remote->m_addr_scope == ossia::net::address_scope::relative && !m_matchers.empty())
        remote->register_node(m_matchers);
      else
        ossia_register(remote);

      ossia::remove_erase(children, child);
    }
  }

  for (auto view : ossia_max::instance().nr_views.copy())
  {
    ossia_register(view);
  }

  // then try to register quarantinized remote
  for (auto remote : ossia_max::instance().nr_remotes.copy())
  {
    ossia_register(remote);
  }
}

bool model::unregister()
{
  save_children_state();
  m_matchers.clear();

  auto nodes = find_parent_nodes();
  auto patcher = get_patcher(&m_object);
  register_objects_in_patcher_recursively(patcher, this, nodes);

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
