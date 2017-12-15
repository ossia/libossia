// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/network/dataspace/dataspace_visitors.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia-max/src/utils.hpp>

using namespace ossia::max;

extern "C" void ossia_parameter_setup()
{
  // instantiate the ossia.parameter class
  auto c = class_new( "ossia.parameter",
      (method)parameter::create,
      (method)parameter::destroy,
      (long)sizeof(ossia::max::parameter), 0L,
      A_GIMME, 0);

  parameter_base::class_setup(c);

  class_addmethod(
      c, (method)parameter::assist,
      "assist", A_CANT, 0);
  class_addmethod(
      c, (method)parameter::notify,
      "notify", A_CANT, 0);
  class_addmethod(
      c, (method)parameter_base::push_default_value,
      "loadbang", A_CANT, 0);

  class_addmethod(c, (method) address_mess_cb<parameter>, "address",   A_SYM, 0);
  class_addmethod(c, (method) parameter_base::get_mess_cb, "get", A_SYM, 0);

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
  auto x = make_ossia<parameter>();

  if (x)
  {
    // make outlets
    x->m_dumpout
        = outlet_new(x, NULL); // anything outlet to dump parameter state

    x->m_data_out = outlet_new(x, NULL); // anything outlet to output data

    // initialize attributes
    x->m_access_mode = gensym("bi");
    x->m_bounding_mode = gensym("free");

    x->m_clock = clock_new(x, (method)parameter_base::push_default_value);
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
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
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
    else if ( attrname == gensym("clip") )
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
    else if ( attrname == gensym("mode") )
      x->set_access_mode();
    else if ( attrname == gensym("repetitions") )
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

bool parameter::register_node(const std::vector<t_matcher>& nodes)
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

bool parameter::do_registration(const std::vector<t_matcher>& matchers)
{
  unregister(); // we should unregister here because we may have add a node
                // between the registered node and the parameter


  for (auto& m : matchers)
  {
    auto node = m.get_node();
    m_parent_node = node;

    auto nodes = ossia::net::create_nodes(*node, m_name->s_name);

    for (auto n : nodes)
    {
      auto local_param = ossia::try_setup_parameter(m_type->s_name, *n);

      if (!local_param)
      {
        object_error(
              (t_object*)this,
              "type should one of: float, symbol, int, vec2f, "
              "vec3f, vec4f, bool, list, char");

        return false;
      }

      ossia::net::set_priority(local_param->get_node(), m_priority);

      ossia::net::set_disabled(local_param->get_node(), !m_enable);

      ossia::net::set_hidden(local_param->get_node(), m_hidden);

      m_matchers.emplace_back(n, this);
    }
  }

  fill_selection();

  set_description();
  set_tags();
  set_access_mode();
  set_unit();
  set_bounding_mode();
  set_range();
  set_minmax();
  set_default();
  set_repetition_filter();

  clock_delay(m_clock, 0);

  return (!m_matchers.empty() || m_is_pattern);
}

bool parameter::unregister()
{
  clock_unset(m_clock);
  clock_unset(m_poll_clock);

  m_matchers.clear();

  for (auto remote : remote::quarantine().copy())
  {
    max_object_register<ossia::max::remote>(static_cast<ossia::max::remote*>(remote));
  }

  object_quarantining(this);

  return true;
}

ossia::safe_set<parameter *> &parameter::quarantine()
{
  return ossia_max::instance().parameter_quarantine;
}

} // max namespace
} // ossia namespace
