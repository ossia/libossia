// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// A starter for Pd objects
#include "model.hpp"
#include "parameter.hpp"
#include "remote.hpp"
#include "utils.hpp"
#include "view.hpp"
#include <ossia/network/base/node_attributes.hpp>

namespace ossia
{
namespace pd
{

static void model_free(t_model* x);

bool t_model::register_node(ossia::net::node_base* node)
{
  bool res = do_registration(node);
  if (res)
  {
    register_children();
  }
  else
    obj_quarantining<t_model>(this);

  return res;
}

bool t_model::do_registration(ossia::net::node_base* node)
{

  if (x_node && x_node->get_parent() == node)
    return true; // already register to this node;
  unregister();  // we should unregister here because we may have add a node
                 // between the registered node and the parameter
  if (!node)
    return false;

  std::string name(x_name->s_name);

  x_parent_node = node;

  if (node->find_child(name))
  { // we have to check if a node with the same name already exists to avoid
    // auto-incrementing name
    std::vector<t_obj_base*> obj
        = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.model");
    for (auto v : obj)
    {
      if (v->x_otype == Type::param)
      {
        t_param* param = (t_param*)v;
        if (std::string(param->x_name->s_name) == name)
        {
          param->unregister(); // if we already have a t_param node of that
                               // name, unregister it
          // we will register it again after node creation
          continue;
        }
      }
    }
  }

  x_node = &ossia::net::create_node(*node, name);
  x_node->about_to_be_deleted.connect<t_model, &t_model::is_deleted>(this);

  set_priority();
  set_description();
  set_tags();

  return true;
}

void t_model::register_children()
{
  obj_dequarantining<t_model>(this);
  std::vector<t_obj_base*> obj
      = find_child_to_register(this, x_obj.o_canvas->gl_list, "ossia.model");
  for (auto v : obj)
  {
    if (v->x_otype == Type::model)
    {
      t_model* model = (t_model*)v;
      if (model == this)
      {
        // not registering itself
        continue;
      }
      model->register_node(x_node);
    }
    else if (v->x_otype == Type::param)
    {
      t_param* param = (t_param*)v;
      param->register_node(x_node);
    }
    else if (v->x_otype == Type::remote)
    {
      t_remote* remote = (t_remote*)v;
      remote->register_node(x_node);
    }
  }

  for (auto view : t_view::quarantine().copy())
  {
    obj_register(static_cast<t_view*>(view));
  }

  // then try to register quarantinized remote
  for (auto remote : t_remote::quarantine().copy())
  {
    obj_register(static_cast<t_remote*>(remote));
  }
}

bool t_model::unregister()
{

  clock_unset(x_regclock);
  if (!x_node)
    return true; // not registered

  x_node->about_to_be_deleted.disconnect<t_model, &t_model::is_deleted>(this);

  if (x_node && x_node->get_parent())
  {
    x_node->get_parent()->remove_child(*x_node); // this calls isDeleted() on
                                                 // each registered child and
                                                 // put them into quarantine
  }

  // we can't register children to parent node
  // because it might be deleted soon
  // (when removing root device for example)

  x_node = nullptr;

  obj_quarantining<t_model>(this);

  register_children();

  return true;
}

void t_model::set_priority()
{
  // TODO why this doesn't work
  if (x_node) ossia::net::set_priority(*x_node, x_priority);
}

void t_model::set_description()
{
  std::stringstream description;
  for (int i = 0; i < x_description_size; i++)
  {
    switch(x_description[i].a_type)
    {
      case A_SYMBOL:
        description << x_description[i].a_w.w_symbol->s_name << " ";
        break;
      case A_FLOAT:
        {
          description << x_description[i].a_w.w_float << " ";
          break;
        }
      default:
        ;
    }
  }

  if(x_node) ossia::net::set_description(*x_node, description.str());
}

void t_model::set_tags()
{
  std::vector<std::string> tags;
  for (int i = 0; i < x_tags_size; i++)
  {
    switch(x_tags[i].a_type)
    {
      case A_SYMBOL:
        tags.push_back(x_tags[i].a_w.w_symbol->s_name);
        break;
      case A_FLOAT:
        {
          std::stringstream ss;
          ss << x_tags[i].a_w.w_float;
          tags.push_back(ss.str());
          break;
        }
      default:
        ;
    }
  }

  if (x_node) ossia::net::set_tags(*x_node, tags);
}


t_pd_err model_notify(t_model*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
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

void model_get_priority(t_model*x)
{
  t_atom a;
  SETFLOAT(&a, x->x_priority);
  outlet_anything(x->x_dumpout, gensym("priority"), 1, &a);
}

void model_get_tags(t_model*x)
{
  outlet_anything(x->x_dumpout, gensym("tags"),
                  x->x_tags_size, x->x_tags);
}

void model_get_description(t_model*x)
{
  outlet_anything(x->x_dumpout, gensym("description"),
                  x->x_description_size, x->x_description);
}

ossia::safe_vector<t_model*>& t_model::quarantine()
{
  static ossia::safe_vector<t_model*> quarantine;
  return quarantine;
}

void t_model::is_deleted(const net::node_base& n)
{
  if (!x_dead)
  {
    x_node->about_to_be_deleted.disconnect<t_model, &t_model::is_deleted>(this);
    x_node = nullptr;
    obj_quarantining<t_model>(this);
  }
}

static void* model_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_model* x = (t_model*)eobj_new(ossia_pd.model);
  if(x)
  {
    ossia_pd.models.push_back(x);

    x->x_otype = Type::model;

    t_binbuf* d = binbuf_via_atoms(argc, argv);
    if (d)
    {
      x->x_dumpout = outlet_new((t_object*)x, gensym("dumpout"));
      x->x_regclock = clock_new(x, (t_method)obj_register<t_model>);

      if (argc != 0 && argv[0].a_type == A_SYMBOL)
      {
        x->x_name = atom_getsymbol(argv);
        x->x_addr_scope = get_address_scope(x->x_name->s_name);
      }
      else
      {
        x->x_name = gensym("untitledModel");
        pd_error(x, "You have to pass a name as the first argument");
      }

      x->x_node = nullptr;
      x->x_parent_node = nullptr;

      ebox_attrprocess_viabinbuf(x, d);

      // we need to delay registration because object may use patcher hierarchy
      // to check address validity
      // and object will be added to patcher's objects list (aka canvas g_list)
      // after model_new() returns.
      // 0 ms delay means that it will be perform on next clock tick
      clock_delay(x->x_regclock, 0);
    }

    if (find_peer(x))
    {
      error(
            "Only one [ø.model]/[ø.view] intance per patcher is allowed.");
      model_free(x);
      x = nullptr;
    }
  }

  return x;
}

static void model_free(t_model* x)
{
  x->x_dead = true;
  x->unregister();
  obj_dequarantining<t_model>(x);
  ossia_pd::instance().models.remove_all(x);
  clock_free(x->x_regclock);
}

extern "C" void setup_ossia0x2emodel(void)
{
  t_eclass* c = eclass_new(
      "ossia.model", (method)model_new, (method)model_free,
      (short)sizeof(t_model), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)model_new,gensym("ø.model"), A_GIMME, 0);

    eclass_addmethod(c, (method)obj_dump<t_model>, "dump", A_NULL, 0);
    eclass_addmethod(c, (method)obj_namespace, "namespace", A_NULL, 0);
    eclass_addmethod(c, (method)obj_set, "set", A_GIMME, 0);
    eclass_addmethod(c, (method) model_notify,     "notify",   A_NULL,  0);

    CLASS_ATTR_ATOM_VARSIZE(c, "description", 0, t_model, x_description, x_description_size, OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "tags", 0, t_model, x_tags, x_tags_size, OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_INT(c, "priority", 0, t_param, x_priority);

    eclass_addmethod(c, (method) model_get_priority,          "getpriority",          A_NULL,  0);
    eclass_addmethod(c, (method) model_get_tags,              "gettags",              A_NULL,  0);
    eclass_addmethod(c, (method) model_get_description,       "getdescription",       A_NULL,  0);
    eclass_addmethod(c, (method) obj_get_address,             "getaddress",           A_NULL,  0);
    eclass_addmethod(c, (method) obj_preset,                  "preset",               A_GIMME, 0);


    // eclass_register(CLASS_OBJ,c); // disable property dialog since it's
    // buggy
  }
  auto& ossia_pd = ossia_pd::instance();
  ossia_pd.model = c;
}
} // pd namespace
} // ossia namespace
