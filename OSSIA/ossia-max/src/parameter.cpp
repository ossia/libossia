// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia-max/src/utils.hpp>

using namespace ossia::max;

extern "C" void ossia_parameter_setup()
{
  // instantiate the ossia.parameter class
  auto c = class_new(
      "ossia.parameter", (method)parameter::create,
      (method)parameter::destroy, (long)sizeof(ossia::max::parameter), 0L,
      A_GIMME, 0);

  parameter_base::class_setup(c);

  class_addmethod(
      c, (method)parameter::assist,
      "assist", A_CANT, 0);
  class_addmethod(
      c, (method)parameter::notify,
      "notify", A_CANT, 0);
  class_addmethod(c, (method) parameter::push_default_value, "reset", A_NOTHING,  0);

  // TODO add a reset method

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia::max::ossia_max::instance();
  ossia_library.ossia_parameter_class = c;

}

namespace ossia
{
namespace max
{

void* parameter::create(t_symbol* s, long argc, t_atom* argv)
{
  auto& ossia_library = ossia::max::ossia_max::instance();
  // ugly hack while waiting for C++ Max API
  auto place = object_alloc(ossia_library.ossia_parameter_class);
  t_object tmp;
  memcpy(&tmp, place, sizeof(t_object));
  parameter* x = new(place) parameter();
  memcpy(x, &tmp, sizeof(t_object));


  if (x)
  {
    // make outlets
    x->m_dumpout
        = outlet_new(x, NULL); // anything outlet to dump parameter state

    x->m_data_out = outlet_new(x, NULL); // anything outlet to output data

    // initialize attributes
    x->m_access_mode = gensym("bi");
    x->m_bounding_mode = gensym("free");

    x->m_clock = clock_new(x, (method)parameter::push_default_value);
    x->m_poll_clock = clock_new(x, (method) parameter_base::output_value);

    x->m_otype = object_class::param;

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
      x->m_name = gensym("untitledParameter");
      return x;
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    // Register object to istself so it can receive notification when attribute changed
    // This is not documented anywhere, please look at :
    // https://cycling74.com/forums/notify-when-attribute-changes
    object_attach_byptr_register(x, x, CLASS_BOX);

    // start registration
    max_object_register<parameter>(x);
    ossia_max::instance().parameters.push_back(x);
  }

  return x;
}

void parameter::destroy(parameter* x)
{
  x->unregister();
  object_dequarantining<parameter>(x);
  ossia_max::instance().parameters.remove_all(x);
  object_free(x->m_clock);
  object_free(x->m_poll_clock);
  outlet_delete(x->m_data_out);
  outlet_delete(x->m_dumpout);
  x->~parameter();
}

void parameter::assist(parameter* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
  {
    sprintf(s, "All purpose input");
  }
  else
  {
    switch(a)
    {
      case 0:
        sprintf(s, "Data output");
        break;
      case 1:
        sprintf(s, "Dumpout");
        break;
      default:
        ;
    }
  }
}

t_max_err parameter::notify(parameter *x, t_symbol *s,
                       t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  if (msg == gensym("attr_modified")) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if( attrname == gensym("range") )
      x->set_range();
    else if ( attrname == gensym("bounding_mode") )
      x->set_bounding_mode();
    else if ( attrname == gensym("min") || attrname == gensym("max") )
      x->set_minmax();
    else if ( attrname == gensym("default") )
      x->set_default();
    else if ( attrname == gensym("unit") )
      x->set_unit();
    else if ( attrname == gensym("hidden") )
      x->set_hidden();
    else if ( attrname == gensym("priority") )
      x->set_priority();
    else if ( attrname == gensym("access_mode") )
      x->set_access_mode();
    else if ( attrname == gensym("repetition_filter") )
      x->set_repetition_filter();
    else if ( attrname == gensym("tags") )
      x->set_tags();
    else if ( attrname == gensym("description") )
      x->set_description();
    else if ( attrname == gensym("enable") )
      x->set_enable();
    else if ( attrname == gensym("type") )
      x->set_type();
    else if ( s == gensym("mute") )
      x->set_mute();
  }
  return 0;
}

void parameter::push_default_value(parameter* x)
{
  if ( x->m_default_size > 0 )
    parameter_base::push(x, nullptr, x->m_default_size, x->m_default);
}

bool parameter::register_node(const std::vector<ossia::net::node_base*>& nodes)
{
  bool res = do_registration(nodes);
  if (res)
  {
    object_dequarantining<parameter>(this);
    for (auto remote : remote::quarantine().copy())
    {
      max_object_register<ossia::max::remote>(static_cast<ossia::max::remote*>(remote));
    }

    clock_delay(m_poll_clock,1);
  }
  else
    object_quarantining<parameter>(this);

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
        object_error(
              (t_object*)this,
              "type should one of: float, symbol, int, vec2f, "
              "vec3f, vec4f, bool, list, char");
      }
      if (!local_param)
        return false;

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
    set_repetition_filter();
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

  for (auto remote : remote::quarantine().copy())
  {
    max_object_register<ossia::max::remote>(static_cast<ossia::max::remote*>(remote));
  }

  object_quarantining(this);

  return true;
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
        object_error((t_object*)this, "wrong unit: %s", m_unit->s_name);
    }
  }
}

void parameter::set_rate()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_refresh_rate(*node,m_rate);
  }
}

void parameter::set_mute()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_muted(*node,m_mute);
  }
}

void parameter::update_attribute(parameter* x, ossia::string_view attribute)
{
  if ( attribute == ossia::net::text_refresh_rate() ){
    parameter::get_rate(x);
  } else if ( attribute == ossia::net::text_muted() ){
    parameter::get_mute(x);
  } else if ( attribute == ossia::net::text_unit() ){
    parameter::get_unit(x);
  } else {
    parameter_base::update_attribute(x, attribute);
  }
}

void parameter::get_rate(parameter*x)
{
  if (!x->m_matchers.empty())
  {
    ossia::net::node_base* node = x->m_matchers[0].get_node();
    auto rate = ossia::net::get_refresh_rate(*node);

    if (rate)
    {
      x->m_rate = *rate;
    }
  }
}

void parameter::get_mute(parameter*x)
{
  if (!x->m_matchers.empty())
  {
    ossia::net::node_base* node = x->m_matchers[0].get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    x->m_mute = param->get_muted();
  }
}

void parameter::get_unit(parameter*x)
{
  if (!x->m_matchers.empty())
  {
    ossia::net::node_base* node = x->m_matchers[0].get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    std::string unit = ossia::get_pretty_unit_text(param->get_unit());
    x->m_unit = gensym(unit.c_str());
  }
}

ossia::safe_vector<parameter*>& parameter::quarantine()
{
  return ossia_max::instance().parameter_quarantine;
}

} // max namespace
} // ossia namespace
