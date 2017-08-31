// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "parameter.hpp"
#include "device.hpp"
#include "model.hpp"
#include "remote.hpp"
#include "utils.hpp"
#include "ossia-pd.hpp"
#include "ossia_obj_base.hpp"

#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <limits>
#include <sstream>
namespace ossia
{
namespace pd
{

static void parameter_free(t_param* x);

t_param::t_param():
  t_object_base{ossia_pd::param}
{ }

bool t_param::register_node(std::vector<ossia::net::node_base*> node)
{
  bool res = do_registration(node);
  if (res)
  {
    obj_dequarantining<t_param>(this);
    for (auto remote : t_remote::quarantine().copy())
    {
      obj_register<t_remote>(static_cast<t_remote*>(remote));
    }

    clock_delay(m_poll_clock,1);
  }
  else
    obj_quarantining<t_param>(this);

  return res;
}

bool t_param::do_registration(std::vector<ossia::net::node_base*> _nodes)
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
  }

  clock_delay(m_clock, 0);

  return true;
}

bool t_param::unregister()
{
  clock_unset(m_clock);
  clock_unset(m_poll_clock);

  m_matchers.clear();
  m_nodes.clear();

  for (auto remote : t_remote::quarantine().copy())
  {
    obj_register<t_remote>(static_cast<t_remote*>(remote));
  }

  obj_quarantining<t_param>(this);

  return true;
}

ossia::safe_vector<t_param*>& t_param::quarantine()
{
    return ossia_pd::instance().parameter_quarantine;
}

static void push_default_value(t_param* x)
{
  if ( x->m_default_size > 0 )
    t_object_base::push(x, nullptr, x->m_default_size, x->m_default);
}

static void* parameter_new(t_symbol* name, int argc, t_atom* argv)
{
  auto& ossia_pd = ossia_pd::instance();
  t_param* x = new t_param();
  // t_param* ox = (t_param*)eobj_new(ossia_pd.param);

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
    x->m_poll_clock = clock_new(x, (t_method)t_object_base::output_value);

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

    obj_register<t_param>(x);
  }

  return (x);
}

void t_param::set_access_mode()
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
      pd_error(this, "unknown access mode: %s", access_mode.c_str());
    }
  }
}

void t_param::set_repetition_filter()
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

void t_param::set_description()
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

  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_description(*node, description.str());
  }
}

void t_param::set_tags()
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

  for (t_matcher& m : m_matchers)
    ossia::net::set_tags(*m.get_node(), tags);
}

void t_param::set_priority()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_priority(*node, m_priority);
  }
}

void t_param::set_enable()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_disabled(*node, !m_enable);
  }
}

void t_param::set_type()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();
    param->set_value_type(symbol2val_type(m_type));
  }
}

void t_param::set_hidden()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_hidden(*node, m_hidden);
  }
}

void t_param::set_unit()
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

void t_param::set_minmax(){
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

void t_param::set_range()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    if ( param->get_value_type() == ossia::val_type::STRING )
    {
      std::vector<std::string> senum;
      for ( int i = 0; i < m_range_size; i++)
      {
        if (m_range[i].a_type == A_SYMBOL)
          senum.push_back(m_range[i].a_w.w_symbol->s_name);
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
            std::array<float, OSSIA_PD_MAX_ATTR_SIZE> min, max;
            min.fill(m_range[0].a_w.w_float);
            max.fill(m_range[1].a_w.w_float);
            omin.assign(min.begin(), min.end());
            omax.assign(max.begin(), max.end());
            param->set_domain(ossia::make_domain(omin,omax));
          }
      }
    }
  }
}

void t_param::set_bounding_mode()
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
      pd_error(this, "unknown bounding mode: %s", bounding_mode.c_str());
    }
  }
}

void t_param::set_default()
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
          if (m_default[0].a_type == A_SYMBOL )
          {
            ossia::net::set_default_value(*node, m_default[0].a_w.w_symbol->s_name);
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

void parameter_get_range(t_param*x)
{
  outlet_anything(x->m_dumpout, gensym("range"), x->m_range_size, x->m_range);
}

void parameter_get_min(t_param*x)
{
  outlet_anything(x->m_dumpout, gensym("min"), x->m_min_size, x->m_min);
}

void parameter_get_max(t_param*x)
{
  outlet_anything(x->m_dumpout, gensym("max"), x->m_max_size, x->m_max);
}

void parameter_get_bounding_mode(t_param*x)
{
  t_atom a;
  SETSYMBOL(&a,x->m_bounding_mode);
  outlet_anything(x->m_dumpout, gensym("bounding_mode"), 1, &a);
}

void parameter_get_default(t_param*x)
{
  outlet_anything(x->m_dumpout, gensym("default"),
                  x->m_default_size, x->m_default);
}

void parameter_get_unit(t_param*x)
{
  t_atom a;
  SETSYMBOL(&a,x->m_unit);
  outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
}

void parameter_get_type(t_param*x)
{
  t_atom a;
  SETSYMBOL(&a,x->m_type);
  outlet_anything(x->m_dumpout, gensym("type"), 1, &a);
}

void parameter_get_hidden(t_param*x)
{
  t_atom a;
  SETFLOAT(&a, x->m_hidden);
  outlet_anything(x->m_dumpout, gensym("hidden"), 1, &a);
}

void parameter_get_priority(t_param*x)
{
  t_atom a;
  SETFLOAT(&a, x->m_priority);
  outlet_anything(x->m_dumpout, gensym("priority"), 1, &a);
}

void parameter_get_access_mode(t_param*x)
{
  t_atom a;
  SETSYMBOL(&a, x->m_access_mode);
  outlet_anything(x->m_dumpout, gensym("access_mode"), 1, &a);
}

void parameter_get_repetition_filter(t_param*x)
{
  t_atom a;
  SETFLOAT(&a, x->m_repetition_filter);
  outlet_anything(x->m_dumpout, gensym("repetition_filter"), 1, &a);
}

void parameter_get_tags(t_param*x)
{
  outlet_anything(x->m_dumpout, gensym("tags"),
                  x->m_tags_size, x->m_tags);
}

void parameter_get_description(t_param*x)
{
  outlet_anything(x->m_dumpout, gensym("description"),
                  x->m_description_size, x->m_description);
}

void parameter_get_enable(t_param*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_enable);
  outlet_anything(x->m_dumpout, gensym("enable"), 1, &a);
}

void parameter_get_mute(t_param*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_mute);
  outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
}

void parameter_get_poll_interval(t_param*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_poll_interval);
  outlet_anything(x->m_dumpout, gensym("poll_interval"), 1, &a);
}

t_pd_err parameter_notify(t_param*x, t_symbol*s, t_symbol* msg, void* sender, void* data)
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

  }
  return 0;
}

static void parameter_free(t_param* x)
{
  x->m_dead = true;
  x->unregister();
  obj_dequarantining<t_param>(x);
  ossia_pd::instance().params.remove_all(x);

  outlet_free(x->m_dataout);
  outlet_free(x->m_dumpout);

  clock_free(x->m_clock);
  clock_free(x->m_poll_clock);

  x->~t_param();
}

extern "C" void setup_ossia0x2eparam(void)
{
  t_eclass* c = eclass_new(
      "ossia.param", (method)parameter_new, (method)parameter_free,
      (short)sizeof(t_param), CLASS_DEFAULT, A_GIMME, 0);

  if (c)
  {
    class_addcreator((t_newmethod)parameter_new,gensym("ø.param"), A_GIMME, 0);

    eclass_addmethod(c, (method) t_object_base::push, "anything", A_GIMME, 0);
    eclass_addmethod(c, (method) t_object_base::bang, "bang",     A_NULL,  0);
    eclass_addmethod(c, (method) obj_dump<t_param>,   "dump",     A_NULL,  0);
    eclass_addmethod(c, (method) parameter_notify,    "notify",   A_NULL,  0);
    // TODO should we do something else with reset (like resetting all attributes)
    eclass_addmethod(c, (method) push_default_value,   "reset",    A_NULL,  0);

    CLASS_ATTR_SYMBOL(c, "type", 0, t_param, m_type);
    CLASS_ATTR_SYMBOL(c, "unit", 0, t_param, m_unit);
    CLASS_ATTR_SYMBOL(c, "bounding_mode", 0, t_param, m_bounding_mode);
    CLASS_ATTR_SYMBOL(c, "access_mode", 0, t_param, m_access_mode);
    CLASS_ATTR_ATOM_VARSIZE(c, "description", 0, t_param, m_description, m_description_size, OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "tags", 0, t_param, m_tags, m_tags_size, OSSIA_PD_MAX_ATTR_SIZE);

    CLASS_ATTR_ATOM_VARSIZE(c, "default",           0, t_param, m_default, m_default_size, OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "range",             0, t_param, m_range,   m_range_size,   OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "min",               0, t_param, m_min,     m_min_size,     OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_ATOM_VARSIZE(c, "max",               0, t_param, m_max,     m_max_size,     OSSIA_PD_MAX_ATTR_SIZE);
    CLASS_ATTR_FLOAT       (c, "repetition_filter", 0, t_param, m_repetition_filter);
    CLASS_ATTR_INT(         c, "priority",          0, t_param, m_priority);
    CLASS_ATTR_INT(         c, "hidden",            0, t_param, m_hidden);
    CLASS_ATTR_INT(         c, "enable",            0, t_param, m_enable);
    CLASS_ATTR_INT(         c, "mute",              0, t_param, m_mute);
    CLASS_ATTR_INT(         c, "poll_interval",     0, t_param, m_poll_interval);

    CLASS_ATTR_DEFAULT(c, "type",          0, "float");
    CLASS_ATTR_DEFAULT(c, "bounding_mode", 0, "free");
    CLASS_ATTR_DEFAULT(c, "access_mode",   0, "bi");

    // attributes getter
    eclass_addmethod(c, (method) parameter_get_range,             "getrange",             A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_min,               "getmin",               A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_max,               "getmax",               A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_bounding_mode,     "getbounding_mode",     A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_default,           "getdefault",           A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_unit,              "getunit",              A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_type,              "gettype",              A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_hidden,            "gethidden",            A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_priority,          "getpriority",          A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_access_mode,       "getaccess_mode",       A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_repetition_filter, "getrepetition_filter", A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_tags,              "gettags",              A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_description,       "getdescription",       A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_enable,            "getenable",            A_NULL, 0);

    eclass_addmethod(c, (method) parameter_get_mute,              "getmute",              A_NULL, 0);
    eclass_addmethod(c, (method) parameter_get_poll_interval,     "getpollinterval",      A_NULL, 0);

    eclass_addmethod(c, (method) obj_get_address,                 "getaddress",           A_NULL, 0);

    // eclass_register(CLASS_OBJ, c); // disable property dialog since it's
    // buggy
  }

  ossia_pd::param = c;
}
} // pd namespace
} // ossia namespace
