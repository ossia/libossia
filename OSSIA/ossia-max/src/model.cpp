// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "view.hpp"
#include "utils.hpp"

#include <ossia/network/base/node_attributes.hpp>

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

  class_register(CLASS_BOX, c);
  ossia_library.ossia_model_class = c;
}

namespace ossia
{
namespace max
{

void* model::create(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  auto place = object_alloc(ossia_library.ossia_model_class);

  t_object tmp;
  memcpy(&tmp, place, sizeof(t_object));
  model* x = new(place) model();
  memcpy(x, &tmp, sizeof(t_object));

  if (x)
  {
    // make outlets
    x->m_dumpout = outlet_new(x, NULL); // anything outlet to dump model state

    x->m_description = _sym_nothing;
    x->m_tags_size = 0;
    x->m_otype = object_class::model;

    if(find_peer(x))
    {
      error("You can put only one [ossia.model] or [ossia.view] per patcher");
      model::destroy(x);
      free(x);
      return nullptr;
    }

    x->m_clock = clock_new(
        x, reinterpret_cast<method>(
               static_cast<bool (*)(model*)>(&max_object_register<model>)));

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::max::get_address_scope(x->m_name->s_name);
       }
    }

    if (x->m_name == _sym_nothing)
    {
      object_error((t_object*)x, "needs a name as first argument");
      x->m_name = gensym("untitledModel");
      return x;
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    // we need to delay registration because object may use patcher hierarchy
    // to check address validity
    // and object will be added to patcher's objects list (aka canvas g_list)
    // after model_new() returns.
    // 0 ms delay means that it will be perform on next clock tick
    // defer_low(x,reinterpret_cast<method>(
                //static_cast<bool (*)(t_model*)>(&max_object_register<t_model>)), nullptr, 0, 0L );
    clock_delay(x->m_clock, 1);
    ossia_max::instance().models.push_back(x);
  }

  return (x);
}

void model::destroy(model* x)
{
  x->m_dead = true;
  x->unregister();
  object_dequarantining<model>(x);
  ossia_max::instance().models.remove_all(x);
  if(x->m_clock) object_free(x->m_clock);
  if(x->m_dumpout) outlet_delete(x->m_dumpout);
  x->~model();
}

void model::assist(model* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "I am inlet %ld", a);
  }
  else
  {
    sprintf(s, "I am outlet %ld", a);
  }
}

bool model::register_node(const std::vector<ossia::net::node_base*>& nodes)
{
  bool res = do_registration(nodes);

  if (res)
  {
    register_children();
  }
  else
    object_quarantining<model>(this);

  return res;
}

bool model::do_registration(const std::vector<ossia::net::node_base*>& nodes)
{
  // we should unregister here because we may have add a node between the
  // registered node and the parameter
  unregister();

  std::string name(m_name->s_name);

  for (auto node : nodes)
  {

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
          if (std::string(param->m_name->s_name) == name)
          {
            // if we already have a t_param node of that
            // name, unregister it
            // we will register it again after node creation
            param->unregister();
            continue;
          }
        }
      }
    }

    m_nodes = ossia::net::create_nodes(*node, name);
    for (auto n : m_nodes)
    {
      t_matcher m{n,this};
      m_matchers.push_back(std::move(m));
    }

    set_priority();
    set_description();
    set_tags();
    set_hidden();
  }

  return true;
}

void model::register_children()
{
  object_dequarantining<model>(this);

  std::vector<object_base*> children = find_children_to_register(
      &m_object, get_patcher(&m_object), gensym("ossia.model"));

  for (auto child : children)
  {
    if (child->m_otype == object_class::model)
    {
      ossia::max::model* model = (ossia::max::model*)child;

      // ignore itself
      if (model == this)
        continue;

      max_object_register<ossia::max::model>(model);
    }
    else if (child->m_otype == object_class::param)
    {
      ossia::max::parameter* parameter = (ossia::max::parameter*)child;
      max_object_register<ossia::max::parameter>(parameter);
    }
  }

  for (auto view : view::quarantine().copy())
  {
    max_object_register<ossia::max::view>(static_cast<ossia::max::view*>(view));
  }

  // then try to register qurantinized remote
  for (auto remote : remote::quarantine().copy())
  {
    max_object_register<ossia::max::remote>(static_cast<ossia::max::remote*>(remote));
  }
}

bool model::unregister()
{
  if (m_clock) clock_unset(m_clock);

  m_matchers.clear();
  m_nodes.clear();

  object_quarantining<model>(this);

  register_children();

  return true;
}

ossia::safe_vector<model*>& model::quarantine()
{
  return ossia_max::instance().model_quarantine;
}

} // max namespace
} // ossia namespace
