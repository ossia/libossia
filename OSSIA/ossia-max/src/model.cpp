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
  ossia_library.ossia_model_class = class_new(
      "ossia.model", (method)ossia_model_new, (method)ossia_model_free,
      (long)sizeof(ossia::max::t_model), 0L, A_GIMME, 0);

  class_addmethod(
      ossia_library.ossia_model_class, (method)ossia_model_assist, "assist",
      A_CANT, 0);

  class_addmethod(
      ossia_library.ossia_model_class, (method)t_object_base::relative_namespace,
              "namespace", A_NOTHING, 0);

  CLASS_ATTR_SYM(
      ossia_library.ossia_model_class, "description", 0, t_model,
      m_description);
  CLASS_ATTR_SYM(ossia_library.ossia_model_class, "tags", 0, t_model, m_tags);

  class_register(CLASS_BOX, ossia_library.ossia_model_class);
  class_alias(ossia_library.ossia_model_class, gensym("ø.model"));
}

extern "C" void* ossia_model_new(t_symbol* name, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  t_model* x = (t_model*)object_alloc(ossia_library.ossia_model_class);

  if(find_peers(x))
  {
    error("you can put only one [ossia.model] per patcher");
    object_free(x);
    return nullptr;
  }

  if (x)
  {
    // make outlets
    x->m_dump_out = outlet_new(x, NULL); // anything outlet to dump model state

    x->m_description = _sym_nothing;
    x->m_tags = _sym_nothing;

    x->m_otype = Type::model;

    x->m_regclock = clock_new(
        x, reinterpret_cast<method>(
               static_cast<bool (*)(t_model*)>(&max_object_register<t_model>)));

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if (attrstart && argv)
    {
      if (atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_absolute = std::string(x->m_name->s_name) != ""
                        && x->m_name->s_name[0] == '/';
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
    clock_delay(x->m_regclock, 0);
    ossia_max::instance().models.push_back(x);
  }

  return (x);
}

extern "C" void ossia_model_free(t_model* x)
{
  x->m_dead = true;
  x->unregister();
  object_dequarantining<t_model>(x);
  ossia_max::instance().models.remove_all(x);
  if(x->m_regclock) object_free(x->m_regclock);
  if(x->m_dump_out) outlet_delete(x->m_dump_out);
}

extern "C" void
ossia_model_assist(t_model* x, void* b, long m, long a, char* s)
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

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_model structure functions

bool t_model::register_node(ossia::net::node_base* node)
{
  bool res = do_registration(node);

  if (res)
  {
    object_dequarantining<t_model>(this);

    std::vector<t_object_base*> children = find_children_to_register(
        &m_object, get_patcher(&m_object), gensym("ossia.model"));

    for (auto child : children)
    {
      if (child->m_otype == Type::model)
      {
        t_model* model = (t_model*)child;

        // ignore itself
        if (model == this)
          continue;

        model->register_node(m_node);
      }
      else if (child->m_otype == Type::param)
      {
        t_parameter* parameter = (t_parameter*)child;

        parameter->register_node(m_node);
      }
    }

    for (auto view : t_view::quarantine().copy())
    {
      max_object_register<t_view>(static_cast<t_view*>(view));
    }

    // then try to register qurantinized remote
    for (auto remote : t_remote::quarantine().copy())
    {
      max_object_register<t_remote>(static_cast<t_remote*>(remote));
    }
  }
  else
    object_quarantining<t_model>(this);

  return res;
}

bool t_model::do_registration(ossia::net::node_base* node)
{
  // already register to this node
  if (m_node && m_node->get_parent() == node)
    return true;

  // we should unregister here because we may have add a node between the
  // registered node and the parameter
  unregister();

  if (!node)
    return false;

  // check if a node with the same name already exists to avoid
  // auto-incrementing name
  if (node->find_child(m_name->s_name))
  {
    std::vector<t_object_base*> children_model = find_children_to_register(
        &m_object, get_patcher(&m_object), gensym("ossia.model"));

    for (auto child : children_model)
    {
      if (child->m_otype == Type::param)
      {
        t_parameter* parameter = (t_parameter*)child;

        // if we already have a t_parameter node of that name, unregister it
        // we will register it again after node creation
        if (parameter->m_name == m_name)
        {
          parameter->unregister();
          continue;
        }
      }
    }
  }

  m_node = &ossia::net::create_node(*node, m_name->s_name);
  m_node->about_to_be_deleted.connect<t_model, &t_model::is_deleted>(this);

  ossia::net::set_description(*m_node, m_description->s_name);
  ossia::net::set_tags(*m_node, parse_tags_symbol(m_tags));

  return true;
}

bool t_model::unregister()
{
  if (m_regclock) clock_unset(m_regclock);

  // not registered
  if (!m_node)
    return true;

  m_node->about_to_be_deleted.disconnect<t_model, &t_model::is_deleted>(this);

  if (m_node && m_node->get_parent())
    m_node->get_parent()->remove_child(*m_node); // this calls isDeleted() on
                                                 // each registered child and
                                                 // put them into quarantine

  m_node = nullptr;

  object_quarantining<t_model>(this);

  register_quarantinized();

  return true;
}

void t_model::is_deleted(const ossia::net::node_base& n)
{
  if (!m_dead)
  {
    if (m_node)
    {
      m_node->about_to_be_deleted.disconnect<t_model, &t_model::is_deleted>(
        this);
      m_node = nullptr;
    }
    object_quarantining<t_model>(this);
  }
}

ossia::safe_vector<t_model*>& t_model::quarantine()
{
  static ossia::safe_vector<t_model*> quarantine;
  return quarantine;
}

} // max namespace
} // ossia namespace
