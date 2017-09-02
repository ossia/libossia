// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// A starter for Pd objects
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "utils.hpp"
#include "view.hpp"
#include <ossia/network/common/websocket_log_sink.hpp>
#include <ossia/network/base/node_attributes.hpp>

namespace ossia
{
namespace pd
{

static void model_free(model* x);

model::model():
  t_object_base{ossia_pd::model_class}
{ }

bool model::register_node(const std::vector<ossia::net::node_base*>& nodes)
{
  bool res = do_registration(nodes);
  if (res)
  {
    register_children();
  }
  else
    obj_quarantining<model>(this);

  return res;
}

bool model::do_registration(const std::vector<ossia::net::node_base*>& nodes)
{
  unregister();  // we should unregister here because we may have add a node
                 // between the registered node and the parameter

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
      std::vector<t_object_base*> obj
          = find_child_to_register(this, m_obj.o_canvas->gl_list, "ossia.model");
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
  }

  return true;
}

void model::register_children()
{
  obj_dequarantining<model>(this);
  std::vector<t_object_base*> obj
      = find_child_to_register(this, m_obj.o_canvas->gl_list, "ossia.model");
  for (auto v : obj)
  {
    if (v->m_otype == object_class::model)
    {
      ossia::pd::model* model = (ossia::pd::model*)v;
      if (model == this)
      {
        // not registering itself
        continue;
      }
      model->register_node(m_nodes);
    }
    else if (v->m_otype == object_class::param)
    {
      parameter* param = (parameter*)v;
      param->register_node(m_nodes);
    }
    else if (v->m_otype == object_class::remote)
    {
      ossia::pd::remote* remote = (ossia::pd::remote*)v;
      remote->register_node(m_nodes);
    }
  }

  for (auto view : view::quarantine().copy())
  {
    obj_register(static_cast<ossia::pd::view*>(view));
  }

  // then try to register quarantinized remote
  for (auto remote : remote::quarantine().copy())
  {
    obj_register(static_cast<ossia::pd::remote*>(remote));
  }
}

bool model::unregister()
{

  clock_unset(m_clock);

  m_matchers.clear();

  // we can't register children to parent node
  // because it might be deleted soon
  // (when removing root device for example)

  m_nodes.clear();

  obj_quarantining<model>(this);

  register_children();

  return true;
}

void model::set_priority()
{
  // TODO why this doesn't work
  for (auto n : m_nodes)
    ossia::net::set_priority(*n, m_priority);
}

void model::set_description()
{
  std::stringstream description;
  for (int i = 0; i < m_description_size; i++)
  {
    switch(m_description[i].a_type)
    {
      case A_SYMBOL:
        description << m_description[i].a_w.w_symbol->s_name << " ";
        break;
      case A_FLOAT:
        {
          description << m_description[i].a_w.w_float << " ";
          break;
        }
      default:
        ;
    }
  }

  for (auto n : m_nodes)
    ossia::net::set_description(*n, description.str());
}

void model::set_tags()
{
  std::vector<std::string> tags;
  for (int i = 0; i < m_tags_size; i++)
  {
    switch(m_tags[i].a_type)
    {
      case A_SYMBOL:
        tags.push_back(m_tags[i].a_w.w_symbol->s_name);
        break;
      case A_FLOAT:
        {
          std::stringstream ss;
          ss << m_tags[i].a_w.w_float;
          tags.push_back(ss.str());
          break;
        }
      default:
        ;
    }
  }

  for (auto n : m_nodes)
    ossia::net::set_tags(*n, tags);
}


t_pd_err model_notify(model*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
  if (msg == gensym("attr_modified"))
  {
      if ( s == gensym("priority") )
        x->set_priority();
      else if ( s == gensym("tags") )
        x->set_tags();
      else if ( s == gensym("description") )
        x->set_description();
  }
  return 0;
}

void model_get_priority(model*x)
{
  t_atom a;
  SETFLOAT(&a, x->m_priority);
  outlet_anything(x->m_dumpout, gensym("priority"), 1, &a);
}

void model_get_tags(model*x)
{
  outlet_anything(x->m_dumpout, gensym("tags"),
                  x->m_tags_size, x->m_tags);
}

void model_get_description(model*x)
{
  outlet_anything(x->m_dumpout, gensym("description"),
                  x->m_description_size, x->m_description);
}

ossia::safe_vector<model*>& model::quarantine()
{
    return ossia_pd::instance().model_quarantine;
}

static void* model_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  ossia::pd::model* x = (ossia::pd::model*)eobj_new(ossia_pd.model_class);
  if(x)
  {
    ossia_pd.models.push_back(x);

    x->m_otype = object_class::model;

    t_binbuf* d = binbuf_via_atoms(argc, argv);
    if (d)
    {
      x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
      x->m_clock = clock_new(x, (t_method)obj_register<model>);

      if (argc != 0 && argv[0].a_type == A_SYMBOL)
      {
        t_symbol* address = atom_getsymbol(argv);
        std::string name = replace_brackets(address->s_name);
        x->m_name = gensym(name.c_str());
        x->m_addr_scope = get_address_scope(x->m_name->s_name);
      }
      else
      {
        x->m_name = gensym("untitledModel");
        pd_error(x, "You have to pass a name as the first argument");
      }

      ebox_attrprocess_viabinbuf(x, d);

      // we need to delay registration because object may use patcher hierarchy
      // to check address validity
      // and object will be added to patcher's objects list (aka canvas g_list)
      // after model_new() returns.
      // clock_set uses ticks as time unit
      clock_set(x->m_clock, 1);
    }

    if (find_peer(x))
    {
      error(
            "Only one [ø.model]/[ø.view] intance per patcher is allowed.");
      model_free(x);
      delete x;
      x = nullptr;
    }
  }

  return x;
}

static void model_free(model* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<model>(x);
  ossia_pd::instance().models.remove_all(x);
  clock_free(x->m_clock);

  x->~model();
}

extern "C" void setup_ossia0x2emodel(void)
{
  t_eclass* c = eclass_new(
      "ossia.model", (method)model_new, (method)model_free,
      (short)sizeof(model), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)model_new,gensym("ø.model"), A_GIMME, 0);

    eclass_addmethod(c, (method)obj_dump<model>, "dump", A_NULL, 0);
    eclass_addmethod(c, (method)obj_namespace, "namespace", A_NULL, 0);
    eclass_addmethod(c, (method)obj_set, "set", A_GIMME, 0);
    eclass_addmethod(c, (method) model_notify,     "notify",   A_NULL,  0);

    CLASS_ATTR_ATOM_VARSIZE(c, "description", 0, model, m_description, m_description_size, OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "tags", 0, model, m_tags, m_tags_size, OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_INT(c, "priority", 0, parameter, m_priority);

    eclass_addmethod(c, (method) model_get_priority,          "getpriority",          A_NULL,  0);
    eclass_addmethod(c, (method) model_get_tags,              "gettags",              A_NULL,  0);
    eclass_addmethod(c, (method) model_get_description,       "getdescription",       A_NULL,  0);
    eclass_addmethod(c, (method) obj_get_address,             "getaddress",           A_NULL,  0);
    eclass_addmethod(c, (method) obj_preset,                  "preset",               A_GIMME, 0);


    // eclass_register(CLASS_OBJ,c); // disable property dialog since it's
    // buggy
  }
  ossia_pd::model_class = c;
}
} // pd namespace
} // ossia namespace
