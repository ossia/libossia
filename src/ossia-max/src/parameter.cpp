// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/network/common/complex_type.hpp>
#include <ossia/network/dataspace/dataspace_visitors.hpp>

#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia-max/src/utils.hpp>

using namespace ossia::max_binding;

extern "C" void ossia_parameter_setup()
{
  // instantiate the ossia.parameter class
  auto c = class_new(
      "ossia.parameter", (method)parameter::create, (method)parameter::destroy,
      (long)sizeof(ossia::max_binding::parameter), 0L, A_GIMME, 0);

  parameter_base::class_setup(c);

  class_addmethod(c, (method)parameter::assist, "assist", A_CANT, 0);
  class_addmethod(c, (method)parameter_base::notify, "notify", A_CANT, 0);

  class_addmethod(c, (method)address_mess_cb<parameter>, "address", A_SYM, 0);
  class_addmethod(c, (method)parameter_base::get_mess_cb, "get", A_SYM, 0);

  class_register(CLASS_BOX, c);

  auto& ossia_library = ossia::max_binding::ossia_max::instance();
  ossia_library.ossia_parameter_class = c;
}

namespace ossia
{
namespace max_binding
{

void* parameter::create(t_symbol* s, long argc, t_atom* argv)
{
  auto x = make_ossia<parameter>();

  if(x)
  {
    critical_enter(0);
    ossia_max::get_patcher_descriptor(x->m_patcher).parameters.push_back(x);

    // make outlets
    x->m_dumpout = outlet_new(x, NULL); // anything outlet to dump parameter state

    x->m_data_out = outlet_new(x, NULL); // anything outlet to output data

    // initialize attributes
    x->m_access_mode = gensym("bi");
    x->m_bounding_mode = gensym("free");

    x->m_otype = object_class::param;

    // parse arguments
    long attrstart = attr_args_offset(argc, argv);

    // check name argument
    x->m_name = _sym_nothing;
    if(attrstart > 0 && argv)
    {
      if(atom_gettype(argv) == A_SYM)
      {
        x->m_name = atom_getsym(argv);
        x->m_addr_scope = ossia::net::get_address_scope(x->m_name->s_name);
      }
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    if(x->m_type != _sym_string && x->m_min_size == 0 && x->m_max_size == 0
       && x->m_range_size == 0)
    {
      // set range if not set by attribute min/max or range
      x->m_range_size = 2;
      A_SETFLOAT(x->m_range, 0);
      A_SETFLOAT(x->m_range + 1, 1);
    }

    // Register object to istself so it can receive notification when attribute changed
    // This is not documented anywhere, please look at :
    // https://cycling74.com/forums/notify-when-attribute-changes
    object_attach_byptr_register(x, x, CLASS_BOX);

    defer_low(x, (method)object_base::loadbang, nullptr, 0, nullptr);

    critical_exit(0);
  }

  return x;
}

void parameter::destroy(parameter* x)
{
  critical_enter(0);
  x->m_dead = true;
  x->unregister();
  outlet_delete(x->m_data_out);
  outlet_delete(x->m_dumpout);
  x->~parameter();
  critical_exit(0);
}

void parameter::assist(parameter* x, void* b, long m, long a, char* s)
{
  if(m == ASSIST_INLET)
  {
    sprintf(s, "Parameter input");
  }
  else
  {
    switch(a)
    {
      case 0:
        sprintf(s, "Parameter value");
        break;
      case 1:
        sprintf(s, "Dumpout");
        break;
      default:;
    }
  }
}

void parameter::do_registration()
{
  clear_and_init_registration();

  set_priority();
  set_hidden();
  set_enable();
  set_description();
  set_tags();
  set_access_mode();
  set_unit();
  set_bounding_mode();
  set_range();
  set_minmax();
  set_default();
  set_rate();
  set_repetition_filter();
  set_recall_safe();

  m_registered = true;

  for(const auto& m : m_matchers)
  {
    auto param = m->get_node()->get_parameter();
    auto val = param->get_default_value();
    auto it = m_value_map.find(m->get_node()->get_name());
    if(it != m_value_map.end())
    {
      val = it->second;
      m_value_map.erase(it);
    }
    if(val)
    {
      // push quiet here because will be fired later following priority
      param->push_value_quiet(*val);
    }
  }
}

bool parameter::unregister()
{
  m_node_selection.clear();
  m_matchers.clear();

  m_registered = false;

  return true;
}

void parameter::save_values()
{
  if(!m_matchers.empty())
  {
    m_value_map.clear();
    for(const auto& m : m_matchers)
    {
      auto n = m->get_node();
      if(auto p = n->get_parameter())
      {
        m_value_map[n->get_name()] = p->value();
      }
    }
  }
}

void parameter::on_device_created_callback(ossia::max_binding::device* dev)
{
  // TODO match the device name with m_name prefix
  // then create_node_from_matcher
}

} // max namespace
} // ossia namespace
