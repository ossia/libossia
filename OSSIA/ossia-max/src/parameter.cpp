// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia-max/src/parameter.hpp>
#include <ossia-max/src/remote.hpp>
#include <ossia-max/src/utils.hpp>

using namespace ossia::max;

#pragma mark -
#pragma mark ossia_parameter class methods

extern "C" void ossia_parameter_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.parameter class
  auto c = class_new(
      "ossia.parameter", (method)ossia_parameter_new,
      (method)ossia_parameter_free, (long)sizeof(ossia::max::parameter), 0L,
      A_GIMME, 0);

  class_addmethod(
      c, (method)ossia_parameter_assist,
      "assist", A_CANT, 0);
  class_addmethod(
      c, (method)object_dump<parameter>,
      "dump", A_NOTHING, 0);
  class_addmethod(
      c, (method)ossia_parameter_notify,
      "notify", A_CANT, 0);

  // TODO add a reset method

  CLASS_ATTR_SYM(
      c, "type", 0, parameter, m_type);
  CLASS_ATTR_ENUM (
      c, "type", 0, "float int bool symbol vec2f vec3f vec4f list impulse");

  CLASS_ATTR_SYM(
      c, "unit", 0, parameter, m_unit);
  CLASS_ATTR_ENUM (
      c, "unit", 0, "gain.linear gain.midigain gain.db gain.db-raw time.second time.bark time.bpm time.cents time.hz time.mel time.midinote time.ms color.argb color.rgba color.rgb color.bgr color.argb8 color.hsv color.cmy8 color.xyz position.cart3D position.cart2D position.spherical position.polar position.openGL position.cylindrical orientation.quaternion orientation.euler orientation.axis angle.degree angle.radian  time.speed distance.m distance.km distance.dm distance.cm distance.mm distance.um distance.nm distance.pm distance.inches distance.feet distance.miles speed.m/s speed.mph speed.km/h speed.kn speed.ft/s speed.ft/h");
  //maybe this enum could be done more properly by retrieving the full list from the dataspace code ?

  CLASS_ATTR_SYM(
      c, "bounding_mode", 0, parameter,
      m_bounding_mode);
  CLASS_ATTR_ENUM (
      c, "bounding_mode", 0, "free clip wrap fold low high");

  CLASS_ATTR_SYM(
      c, "access_mode", 0, parameter,
      m_access_mode);
  CLASS_ATTR_ENUM (
      c, "access_mode", 0, "bi get set");

  CLASS_ATTR_SYM(
      c, "description", 0, parameter,
      m_description);

  CLASS_ATTR_SYM_VARSIZE(
      c, "tags", 0, parameter, m_tags, m_tags_size, 64);

  CLASS_ATTR_ATOM_VARSIZE(
      c, "default", 0, parameter,
      m_default, m_default_size, OSSIA_MAX_MAX_ATTR_SIZE);

  CLASS_ATTR_ATOM_VARSIZE(
      c, "range", 0, parameter,
      m_range, m_range_size, OSSIA_MAX_MAX_ATTR_SIZE);

  CLASS_ATTR_ATOM_VARSIZE(
      c, "min", 0, parameter,
      m_min, m_min_size, OSSIA_MAX_MAX_ATTR_SIZE);

  CLASS_ATTR_ATOM_VARSIZE(
      c, "max", 0, parameter,
      m_max, m_max_size, OSSIA_MAX_MAX_ATTR_SIZE);

  // TODO add enum for boolean param
  CLASS_ATTR_LONG(
      c, "repetition_filter", 0, parameter,
      m_repetition_filter);

  CLASS_ATTR_LONG(
      c, "priority", 0, parameter,
      m_priority);

  CLASS_ATTR_LONG(
      c, "hidden", 0, parameter,
      m_hidden);

  CLASS_ATTR_LONG(
      c, "enable", 0, parameter,
      m_hidden);

  CLASS_ATTR_STYLE(
      c, "repetition_filter", 0, "onoff");
  CLASS_ATTR_STYLE(
      c, "hidden", 0, "onoff");
  CLASS_ATTR_STYLE(
      c, "enable", 0, "onoff");

  class_register(CLASS_BOX, c);

  ossia_library.ossia_parameter_class = c;
}

extern "C" void* ossia_parameter_new(t_symbol* s, long argc, t_atom* argv)
{
  auto& ossia_library = ossia_max::instance();
  parameter* x
      = (parameter*)object_alloc(ossia_library.ossia_parameter_class);

  if (x)
  {
    // make outlets
    x->m_dumpout
        = outlet_new(x, NULL); // anything outlet to dump parameter state

    x->m_data_out = outlet_new(x, NULL); // anything outlet to output data

    // initialize attributes
    atom_setfloat(&x->m_range[0], 0.);
    atom_setfloat(&x->m_range[1], 1.);
    x->m_access_mode = gensym("bi");
    x->m_bounding_mode = gensym("free");
    x->m_unit = gensym("");
    x->m_type = gensym("float");
    x->m_tags_size = 0;
    x->m_description = gensym("");
    x->m_priority = 0;

    x->m_clock = clock_new(x, (method)parameter::push_default_value);

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
        x->m_addr_scope = ossia::max::get_parameter_type(x->m_name->s_name);
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

  std::cout << "new o.param object" << std::endl;

  return x;
}

extern "C" void ossia_parameter_free(parameter* x)
{
  x->unregister();
  object_dequarantining<parameter>(x);
  ossia_max::instance().parameters.remove_all(x);
  object_free(x->m_clock);
  outlet_delete(x->m_data_out);
  outlet_delete(x->m_dumpout);
}

extern "C" void
ossia_parameter_assist(parameter* x, void* b, long m, long a, char* s)
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

extern "C" t_max_err
ossia_parameter_notify(parameter *x, t_symbol *s,
                       t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  post("parameter notify call");

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

  }
  return 0;
}

namespace ossia
{
namespace max
{

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
  }

  clock_delay(m_clock, 0);

  return true;
}

bool parameter::unregister()
{
  clock_unset(m_clock);

  m_matchers.clear();
  m_nodes.clear();

  for (auto remote : remote::quarantine().copy())
  {
    max_object_register<ossia::max::remote>(static_cast<ossia::max::remote*>(remote));
  }

  object_quarantining(this);

  return true;
}

void parameter::set_access_mode()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    auto param = node->get_parameter();

    std::string access_mode = m_access_mode->s_name;
    ossia::transform(access_mode, access_mode.begin(), ::tolower);
    m_access_mode = gensym(access_mode.c_str());

    if (access_mode == "bi" || access_mode == "rw")
      param->set_access(ossia::access_mode::BI);
    else if (access_mode == "get" || access_mode == "r")
      param->set_access(ossia::access_mode::GET);
    else if (access_mode == "set" || access_mode == "w")
      param->set_access(ossia::access_mode::SET);
    else
    {
      object_error((t_object*)this, "unknown access mode: %s", access_mode.c_str());
    }
  }
}

void parameter::set_repetition_filter()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    auto param = node->get_parameter();
    param->set_repetition_filter(
          m_repetition_filter ? ossia::repetition_filter::ON
                              : ossia::repetition_filter::OFF);
  }
}

void parameter::set_description()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_description(*node, m_description->s_name);
  }
}

void parameter::set_tags()
{
  std::vector<std::string> tags;
  for (int i = 0; i < m_tags_size; i++)
  {
    switch(m_tags[i].a_type)
    {
      case A_SYM:
        tags.push_back(m_tags[i].a_w.w_sym->s_name);
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

  for (t_matcher& m : m_matchers)
    ossia::net::set_tags(*m.get_node(), tags);
}

void parameter::set_priority()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_priority(*node, m_priority);
  }
}

void parameter::set_enable()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_disabled(*node, !m_enable);
  }
}

void parameter::set_type()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();
    param->set_value_type(symbol2val_type(m_type));
  }
}

void parameter::set_hidden()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_hidden(*node, m_hidden);
  }
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

void parameter::set_minmax(){
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    std::vector<ossia::value> min = attribute2value(m_min, m_min_size);
    std::vector<ossia::value> max = attribute2value(m_max, m_max_size);

    if (min.empty())
    {
      switch( param->get_value_type() )
      {
        case ossia::val_type::CHAR:
          min = {0};
          break;
        case ossia::val_type::FLOAT:
        case ossia::val_type::INT:
          min = {0.};
          break;
        case ossia::val_type::VEC2F:
          min = {0.,0.};
          break;
        case ossia::val_type::VEC3F:
          min = {0.,0.,0.};
          break;
        case ossia::val_type::VEC4F:
          min = {0.,0.,0.,0.};
          break;
        default:
          ;
      }
    }

    if ( max.empty() )
    {
      switch( param->get_value_type() )
      {
        case ossia::val_type::CHAR:
          min = {255};
          break;
        case ossia::val_type::FLOAT:
        case ossia::val_type::INT:
          min = {1.};
          break;
        case ossia::val_type::VEC2F:
          min = {1.,1.};
          break;
        case ossia::val_type::VEC3F:
          min = {1.,1.,1.};
          break;
        case ossia::val_type::VEC4F:
          min = {1.,1.,1.,1.};
          break;
        default:
          ;
      }
    }

    if (!min.empty() && !max.empty())
      param->set_domain(ossia::make_domain(min,max));
  }
}

void parameter::set_range()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    switch(param->get_value_type())
    {
      case ossia::val_type::INT:
      case ossia::val_type::FLOAT:
      case ossia::val_type::CHAR:
      case ossia::val_type::STRING:
      case ossia::val_type::LIST:
      case ossia::val_type::VEC2F:
      case ossia::val_type::VEC3F:
      case ossia::val_type::VEC4F:
        {

          if ( param->get_value_type() == ossia::val_type::STRING )
          {
            std::vector<std::string> senum;
            for ( int i = 0; i < m_range_size; i++)
            {
              if (m_range[i].a_type == A_SYM)
                senum.push_back(m_range[i].a_w.w_sym->s_name);
              else if (m_range[i].a_type == A_FLOAT)
              {
                std::stringstream ss;
                ss << m_range[i].a_w.w_float;
                senum.push_back(ss.str());
              }
              else
                break;
            }
            param->set_domain(make_domain(senum));
          }
          else if (m_range[0].a_type == A_FLOAT && m_range[1].a_type == A_FLOAT)
          {
            switch( param->get_value_type() )
            {
              case ossia::val_type::INT:
              case ossia::val_type::FLOAT:
              case ossia::val_type::CHAR:
                param->set_domain(
                      ossia::make_domain(m_range[0].a_w.w_float,m_range[1].a_w.w_float));
                break;
              default:
                {
                  std::vector<ossia::value> omin, omax;
                  // TODO check param size
                  std::array<float, OSSIA_MAX_MAX_ATTR_SIZE> min, max;
                  min.fill(m_range[0].a_w.w_float);
                  max.fill(m_range[1].a_w.w_float);
                  omin.assign(min.begin(), min.end());
                  omax.assign(max.begin(), max.end());
                  param->set_domain(ossia::make_domain(omin,omax));
                }
            }
          }
        }
      default:
        ;
    }
  }
}

void parameter::set_bounding_mode()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    std::string bounding_mode = m_bounding_mode->s_name;
    ossia::transform(bounding_mode, bounding_mode.begin(), ::tolower);
    m_bounding_mode = gensym(bounding_mode.c_str());

    if (bounding_mode == "free")
      param->set_bounding(ossia::bounding_mode::FREE);
    else if (bounding_mode == "clip")
      param->set_bounding(ossia::bounding_mode::CLIP);
    else if (bounding_mode == "wrap")
      param->set_bounding(ossia::bounding_mode::WRAP);
    else if (bounding_mode == "fold")
      param->set_bounding(ossia::bounding_mode::FOLD);
    else if (bounding_mode == "low")
      param->set_bounding(ossia::bounding_mode::LOW);
    else if (bounding_mode == "high")
      param->set_bounding(ossia::bounding_mode::HIGH);
    else
    {
      object_error((t_object*)this, "unknown bounding mode: %s", bounding_mode.c_str());
    }
  }
}

void parameter::set_default()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    switch(param->get_value_type())
    {

      case ossia::val_type::VEC4F:
        {
          if (m_default[0].a_type == A_FLOAT && m_default[1].a_type == A_FLOAT
              && m_default[2].a_type == A_FLOAT && m_default[3].a_type == A_FLOAT)
          {
            vec4f vec = make_vec(
                  m_default[0].a_w.w_float, m_default[1].a_w.w_float,
                m_default[2].a_w.w_float, m_default[3].a_w.w_float);
            ossia::net::set_default_value(*node, vec);
          }
          break;
        }
      case ossia::val_type::VEC3F:
        {
          if (m_default[0].a_type == A_FLOAT && m_default[1].a_type == A_FLOAT
              && m_default[2].a_type == A_FLOAT )
          {
            vec3f vec = make_vec(
                  m_default[0].a_w.w_float, m_default[1].a_w.w_float,
                m_default[2].a_w.w_float);
            ossia::net::set_default_value(*node, vec);
          }
          break;
        }
      case ossia::val_type::VEC2F:
        {
          if (m_default[0].a_type == A_FLOAT && m_default[1].a_type == A_FLOAT )
          {
            vec2f vec = make_vec(
                  m_default[0].a_w.w_float, m_default[1].a_w.w_float);
            ossia::net::set_default_value(*node, vec);
          }
          break;
        }
      case ossia::val_type::FLOAT:
      case ossia::val_type::CHAR:
      case ossia::val_type::INT:
      case ossia::val_type::BOOL:
        {
          if (m_default[0].a_type == A_FLOAT )
          {
            ossia::net::set_default_value(*node, m_default[0].a_w.w_float);
          }
          break;
        }
      case ossia::val_type::STRING:
        {
          if (m_default[0].a_type == A_SYM )
          {
            ossia::net::set_default_value(*node, m_default[0].a_w.w_sym->s_name);
          }
          break;
        }
      case ossia::val_type::LIST:
        {
          auto def = attribute2value(m_default, m_default_size);

          ossia::net::set_default_value(*node, def);
          break;
        }
      default:
        ;
    }
  }
}

ossia::safe_vector<parameter*>& parameter::quarantine()
{
  return ossia_max::instance().parameter_quarantine;
}

} // max namespace
} // ossia namespace
