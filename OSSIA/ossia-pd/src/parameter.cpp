// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "parameter.hpp"
#include "device.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "utils.hpp"
#include "ossia-pd.hpp"
#include <ossia-pd/src/object_base.hpp>

#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <limits>
#include <sstream>
namespace ossia
{
namespace pd
{

static void parameter_free(parameter* x);

parameter::parameter():
  parameter_base{ossia_pd::param_class}
{ }

bool parameter::register_node(const std::vector<ossia::net::node_base*>& nodes)
{
  bool res = do_registration(nodes);
  if (res)
  {
    obj_dequarantining<parameter>(this);
    for (auto remote : ossia::pd::remote::quarantine().copy())
    {
      obj_register<ossia::pd::remote>(static_cast<ossia::pd::remote*>(remote));
    }

    clock_delay(m_poll_clock,1);
  }
  else
    obj_quarantining<parameter>(this);

  return res;
}

bool parameter::do_registration(const std::vector<ossia::net::node_base*>& _nodes)
{
  unregister(); // we should unregister here because we may have add a node
                // between the registered node and the parameter


  for (auto node : _nodes)
  {
    m_parent_node = node;

    auto nodes = ossia::net::create_nodes(*node, m_name->s_name);

    for (auto n : nodes)
    {
      ossia::net::parameter_base* local_param{};

      auto type = symbol2val_type(m_type);

      if ( type != ossia::val_type::NONE )
        local_param = n->create_parameter(type);
      else
      {
        pd_error(
              this,
              "type should one of: float, symbol, int, vec2f, "
              "vec3f, vec4f, bool, list, char");
      }
      if (!local_param)
        return false;

      local_param->set_repetition_filter(
            m_repetition_filter ? ossia::repetition_filter::ON
                                : ossia::repetition_filter::OFF);

      ossia::net::set_priority(local_param->get_node(), m_priority);

      ossia::net::set_disabled(local_param->get_node(), !m_enable);

      ossia::net::set_hidden(local_param->get_node(), m_hidden);

      t_matcher matcher{n,this};
      m_matchers.push_back(std::move(matcher));
      m_nodes.push_back(n);
    }

    set_description();
    set_tags();
    set_access_mode();
    set_unit();
    set_bounding_mode();
    set_range();
    set_minmax();
    set_default();
    set_rate();
  }

  clock_delay(m_clock, 0);

  return true;
}

bool parameter::unregister()
{
  clock_unset(m_clock);
  clock_unset(m_poll_clock);

  m_matchers.clear();
  m_nodes.clear();

  for (auto remote : ossia::pd::remote::quarantine().copy())
  {
    obj_register<ossia::pd::remote>(static_cast<ossia::pd::remote*>(remote));
  }

  obj_quarantining<parameter>(this);

  return true;
}

ossia::safe_vector<parameter*>& parameter::quarantine()
{
    return ossia_pd::instance().parameter_quarantine;
}

static void push_default_value(parameter* x)
{
  if ( x->m_default_size > 0 )
    parameter_base::push(x, nullptr, x->m_default_size, x->m_default);
}

static void* parameter_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  parameter* x = new parameter();

  // TODO SANITIZE : memory leak
  t_binbuf* d = binbuf_via_atoms(argc, argv);

  if (x && d)
  {
    ossia_pd.params.push_back(x);
    x->m_otype = object_class::param;

    x->m_dataout = outlet_new((t_object*)x, nullptr);
    x->m_dumpout = outlet_new((t_object*)x, gensym("dumpout"));

    x->m_access_mode = gensym("rw");
    x->m_bounding_mode = gensym("free");
    x->m_unit = gensym("");
    x->m_type = gensym("float");

    x->m_clock = clock_new(x, (t_method)push_default_value);
    x->m_poll_clock = clock_new(x, (t_method)parameter_base::output_value);

    if (argc != 0 && argv[0].a_type == A_SYMBOL)
    {
      t_symbol* address = atom_getsymbol(argv);
      std::string name = replace_brackets(address->s_name);
      x->m_name = gensym(name.c_str());
      x->m_addr_scope = get_address_scope(x->m_name->s_name);
    }
    else
    {
      pd_error(x, "You have to pass a name as the first argument");
      x->m_name = gensym("untitledParam");
    }

    ebox_attrprocess_viabinbuf(x, d);

    // change some attributes names to lower case
    std::string type = x->m_type->s_name;
    ossia::transform(type, type.begin(), ::tolower);
    x->m_type = gensym(type.c_str());

    obj_register<parameter>(x);
  }

  return (x);
}

void parameter::set_unit()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    if ( m_unit !=  gensym("") )
    {
      ossia::unit_t unit = ossia::parse_pretty_unit(m_unit->s_name);
      if (unit)
      {
        param->set_unit(unit);
        // update m_type since set_unit() may have changed it
        auto val_type = param->get_value_type();
        m_type = val_type2symbol(val_type);
      }
      else
        pd_error(this, "wrong unit: %s", m_unit->s_name);
    }
  }
}

void parameter::get_unit(parameter*x)
{
  t_atom a;
  SETSYMBOL(&a,x->m_unit);
  outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
}

void parameter::get_mute(parameter*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_mute);
  outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
}

void parameter::get_rate(parameter*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_rate);
  outlet_anything(x->m_dumpout, gensym("rate"), 1, &a);
}

t_pd_err parameter::notify(parameter*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
{
  if (msg == gensym("attr_modified"))
  {
      if( s == gensym("range") )
        x->set_range();
      else if ( s == gensym("bounding_mode") )
        x->set_bounding_mode();
      else if ( s == gensym("min") || s == gensym("max") )
        x->set_minmax();
      else if ( s == gensym("default") )
        x->set_default();
      else if ( s == gensym("unit") )
        x->set_unit();
      else if ( s == gensym("hidden") )
        x->set_hidden();
      else if ( s == gensym("priority") )
        x->set_priority();
      else if ( s == gensym("access_mode") )
        x->set_access_mode();
      else if ( s == gensym("repetition_filter") )
        x->set_repetition_filter();
      else if ( s == gensym("tags") )
        x->set_tags();
      else if ( s == gensym("description") )
        x->set_description();
      else if ( s == gensym("enable") )
        x->set_enable();
      else if ( s == gensym("type") )
        x->set_type();
      else if ( s == gensym("rate") )
        x->set_rate();

  }
  return 0;
}

static void parameter_free(parameter* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<parameter>(x);
  ossia_pd::instance().params.remove_all(x);

  outlet_free(x->m_dataout);
  outlet_free(x->m_dumpout);

  clock_free(x->m_clock);
  clock_free(x->m_poll_clock);

  x->~parameter();
}

extern "C" void setup_ossia0x2eparam(void)
{
  t_eclass* c = eclass_new(
      "ossia.param", (method)parameter_new, (method)parameter_free,
      (short)sizeof(parameter), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)parameter_new,gensym("ø.param"), A_GIMME, 0);

    eclass_addmethod(c, (method) parameter_base::push, "anything", A_GIMME, 0);
    eclass_addmethod(c, (method) parameter_base::bang, "bang",     A_NULL,  0);
    eclass_addmethod(c, (method) obj_dump<parameter>,   "dump",     A_NULL,  0);
    eclass_addmethod(c, (method) parameter::notify,    "notify",   A_NULL,  0);
    // TODO should we do something else with reset (like resetting all attributes)
    eclass_addmethod(c, (method) push_default_value,   "reset",    A_NULL,  0);

    parameter_base::declare_attributes(c);

    // special attributes
    CLASS_ATTR_DEFAULT(c, "type",          0, "float");
    CLASS_ATTR_DEFAULT(c, "bounding_mode", 0, "free");
    CLASS_ATTR_DEFAULT(c, "access_mode",   0, "bi");

    CLASS_ATTR_SYMBOL(      c, "unit",              0, parameter, m_unit);
    CLASS_ATTR_INT(         c, "mute",              0, parameter, m_mute);
    CLASS_ATTR_FLOAT(       c, "rate",              0, parameter, m_rate);

    eclass_addmethod(c, (method) parameter::get_unit,              "getunit",              A_NULL, 0);
    eclass_addmethod(c, (method) parameter::get_mute,              "getmute",              A_NULL, 0);
    eclass_addmethod(c, (method) parameter::get_rate,              "getrate",              A_NULL, 0);

    eclass_addmethod(c, (method) object_base::get_address,                 "getaddress",           A_NULL, 0);

    // eclass_register(CLASS_OBJ, c); // disable property dialog since it's
    // buggy
  }

  ossia_pd::param_class = c;
}
} // pd namespace
} // ossia namespace
