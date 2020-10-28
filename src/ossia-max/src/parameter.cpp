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
    auto patcher = x->m_patcher;
    ossia_max::instance().patchers[patcher].parameters.push_back(x);
    ossia_max::instance().patchers[patcher].parent_patcher = ossia::max::get_patcher(patcher);

    // make outlets
    x->m_dumpout
        = outlet_new(x, NULL); // anything outlet to dump parameter state

    x->m_data_out = outlet_new(x, NULL); // anything outlet to output data

    // initialize attributes
    x->m_access_mode = gensym("bi");
    x->m_bounding_mode = gensym("free");

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
      return x;
    }

    // process attr args, if any
    attr_args_process(x, argc - attrstart, argv + attrstart);

    if(x->m_type != gensym("string")
            && x->m_min_size == 0
            && x->m_max_size == 0
            && x->m_range_size == 0)
    {
      // set range if not set by attribute min/max or range
      x->m_range_size = 2;
      A_SETFLOAT(x->m_range,0);
      A_SETFLOAT(x->m_range+1,1);
    }

    // Register object to istself so it can receive notification when attribute changed
    // This is not documented anywhere, please look at :
    // https://cycling74.com/forums/notify-when-attribute-changes
    object_attach_byptr_register(x, x, CLASS_BOX);

    // if this is the first object in patcher, this patcher has not been loadbanged
    // and we should register the object,
    // if we are loading a patcher, matcher should be empty and registration does nothing

    std::cout << "patchers: " << patcher << " contains "
              << ossia_max::instance().patchers[patcher].size() << " ossia objects" << std::endl;

    // need to schedule a loadbang because objects only receive a loadbang when patcher loads.
    x->m_reg_clock = clock_new(x, (method) object_base::loadbang);
    clock_set(x->m_reg_clock, 1);

    ossia_max::instance().parameters.push_back(x);
  }

  return x;
}

void parameter::destroy(parameter* x)
{
  auto pat_it = ossia_max::instance().patchers.find(x->m_patcher);
  if(pat_it != ossia_max::instance().patchers.end())
  {
    auto& pat_desc = pat_it->second;
    pat_desc.parameters.remove_all(x);
    if(pat_desc.empty())
    {
      ossia_max::instance().patchers.erase(pat_it);
    }
  }

  x->m_dead = true;
  x->unregister();
  ossia_max::instance().parameters.remove_all(x);
  outlet_delete(x->m_data_out);
  outlet_delete(x->m_dumpout);
  x->~parameter();
}

void parameter::assist(parameter* x, void* b, long m, long a, char* s)
{
  if (m == ASSIST_INLET)
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
      default:
        ;
    }
  }
}

t_max_err parameter::notify(parameter *x, t_symbol *s,
                       t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  if (!x->m_lock && msg == gensym("attr_modified")) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if ( attrname == gensym("unit") )
      x->set_unit();
    else if ( attrname == gensym("type") )
      x->set_type();
    else
      parameter_base::notify(x, s, msg, sender, data);
  }
  return 0;
}

bool parameter::register_node(const std::vector<std::shared_ptr<matcher>>& nodes)
{
  bool res = do_registration(nodes);
  if (res)
  {
    ossia_max::instance().nr_parameters.remove_all(this);
  }
  else
  {
    ossia_max::instance().nr_parameters.push_back(this);
  }

  return res;
}

bool parameter::do_registration(const std::vector<std::shared_ptr<matcher>>& matchers)
{
  object_post(&m_object, "register parameter");

  if(!m_registered)
  {
    m_registered = true;

    // FIXME refactor absolute and global address scope handling
    switch(m_addr_scope)
    {
      case ossia::net::address_scope::absolute:
      case ossia::net::address_scope::global:
        object_error(&m_object, "parameter with glboal/absolute path are not supported yet");
        return true;
      default:
          ;
    }
  }

  m_matchers.clear();
  m_matchers.reserve(matchers.size());

  for (auto& m : matchers)
  {
    auto node = m->get_node();
    m_parent_node = node;

    auto params = ossia::net::find_or_create_parameter(
          *node, m_name->s_name, m_type->s_name);

    for (auto p : params)
    {
      if (!p)
      {
        object_error(
              (t_object*)this,
              "type should one of: float, symbol, int, vec2f, "
              "vec3f, vec4f, bool, list, char");

        return false;
      }

      ossia::net::set_priority(p->get_node(), m_priority);

      ossia::net::set_disabled(p->get_node(), !m_enable);

      ossia::net::set_hidden(p->get_node(), m_invisible);

      m_matchers.emplace_back(std::make_shared<matcher>(&p->get_node(), this));
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
  set_rate();
  set_repetition_filter();
  set_recall_safe();

  if(!ossia_max::instance().registering_nodes) // don't push default value when registering at loadbang
    push_default_value(this);                  // default value will be sent at the end of the global registration

  return (!m_matchers.empty() || m_is_pattern);
}

bool parameter::unregister()
{
  m_node_selection.clear();
  m_matchers.clear();

  ossia_max::instance().nr_parameters.push_back(this);

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

} // max namespace
} // ossia namespace
