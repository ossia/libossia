// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/common/websocket_log_sink.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia-pd/src/model.hpp>
#include <ossia-pd/src/ossia-pd.hpp>
#include <ossia-pd/src/node_base.hpp>
#include <ossia-pd/src/utils.hpp>

namespace ossia
{
namespace pd
{

model::model():
  node_base{ossia_pd::model_class}
{ }

bool model::register_node(const std::vector<ossia::net::node_base*>& nodes)
{
  if (m_dead) return true;
  bool res = do_registration(nodes);
  if (res)
  {
    fill_selection();
    obj_dequarantining<model>(this);
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
      std::vector<object_base*> obj
          = find_child_to_register(this, m_obj.o_canvas->gl_list, "ossia.model");
      for (auto v : obj)
      {
        if (v->m_otype == object_class::param)
        {
          parameter* param = (parameter*)v;
          if (ossia::string_view(param->m_name->s_name) == name)
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
      m_matchers.emplace_back(n, this);
    }

    fill_selection();

    set_priority();
    set_description();
    set_tags();
    set_hidden();
  }

  return true;
}

void model::register_children()
{
  std::vector<object_base*> obj
      = find_child_to_register(this, m_obj.o_canvas->gl_list, "ossia.model");
  for (auto v : obj)
  {
    if (v->m_otype == object_class::model)
    {
      ossia::pd::model* model = (ossia::pd::model*)v;

      // not registering itself
      if (model == this)
        continue;

      obj_register<ossia::pd::model>(model);
    }
    else if (v->m_otype == object_class::param)
    {
      parameter* param = (parameter*)v;
      // param->register_node(m_nodes);
      obj_register<ossia::pd::parameter>(param);
    }
    else if (v->m_otype == object_class::remote)
    {
      ossia::pd::remote* remote = (ossia::pd::remote*)v;
      // remote->register_node(m_nodes);
      obj_register<ossia::pd::remote>(remote);
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
  m_nodes.clear();

  obj_quarantining<model>(this);

  //m_parent_node = find_parent_nodes(x);

  register_children();

  return true;
}


t_pd_err model::notify(model*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
  if (msg == gensym("attr_modified"))
  {
      if ( s == gensym("priority") )
        x->set_priority();
      else if ( s == gensym("tags") )
        x->set_tags();
      else if ( s == gensym("description") )
        x->set_description();
      else if ( s == gensym("hidden") )
        x->set_hidden();
  }
  return 0;
}

ossia::safe_set<model*>& model::quarantine()
{
    return ossia_pd::instance().model_quarantine;
}

void* model::create(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  ossia::pd::model* x = new ossia::pd::model();
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
        x->m_addr_scope = net::get_address_scope(x->m_name->s_name);
      }
      else
      {
        std::string cur = canvas_getcurrent()->gl_name->s_name;
        if(cur.find(".pd") != std::string::npos)
        {
          cur.resize(cur.size() - 3);
        }
        if(cur.empty())
        {
          x->m_name = gensym("unnamedModel");
          pd_error(x, "You have to pass a name as the first argument");
        }
        else
        {
          x->m_name = gensym(cur.c_str());
        }
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
      model::destroy(x);
      free(x);
      x = nullptr;
    }
  }

  return x;
}

void model::destroy(model* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<model>(x);
  ossia_pd::instance().models.remove_all(x);
  clock_free(x->m_clock);
  x->m_clock = nullptr;

  x->~model();
}

extern "C" void setup_ossia0x2emodel(void)
{
  t_eclass* c = eclass_new(
      "ossia.model", (method)model::create, (method)model::destroy,
      (short)sizeof(model), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)model::create,gensym("ø.model"), A_GIMME, 0);

    node_base::class_setup(c);

    eclass_addmethod(c, (method) model::notify,     "notify",   A_NULL,  0);
    eclass_addmethod(c, (method) address_mess_cb<model>, "address",   A_SYMBOL, 0);

    // eclass_register(CLASS_OBJ,c); // disable property dialog since it's
    // buggy
  }
  ossia_pd::model_class = c;
}
} // pd namespace
} // ossia namespace
