// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-pd/src/parameter_base.hpp>

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia-pd/src/utils.hpp>

#include <sstream>
#include <algorithm>

namespace ossia {
namespace pd {

parameter_base::parameter_base(t_eclass* x)
  : object_base{x}
{
  m_range_size = 2;
  SETFLOAT(m_range,0);
  SETFLOAT(m_range+1,1);
}

void parameter_base::update_attribute(parameter_base* x, ossia::string_view attribute)
{
  if ( attribute == ossia::net::text_value_type() ){
    get_type(x);
  } else if ( attribute == ossia::net::text_domain() ){
    // get_domain(x);
    // logpost(x,2,"update domain attribute");
  } else if ( attribute == ossia::net::text_access_mode() ){
    get_access_mode(x);
  } else if ( attribute == ossia::net::text_bounding_mode() ){
    get_bounding_mode(x);
  } else if ( attribute == ossia::net::text_disabled() ){
    get_enable(x);
  } else if ( attribute == ossia::net::text_repetition_filter() ){
    get_repetition_filter(x);
  } else if ( attribute == ossia::net::text_default_value() ) {
    get_default(x);
  } else {
    object_base::update_attribute((node_base*)x, attribute);
  }
}

void parameter_base::set_access_mode()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    auto param = node->get_parameter();

    std::string access_mode = m_access_mode->s_name;
    ossia::transform(access_mode, access_mode.begin(), ::tolower);
    m_access_mode = gensym(access_mode.c_str());

    param->set_access(symbol2access_mode(m_access_mode));
  }
}

void parameter_base::set_repetition_filter()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    auto param = node->get_parameter();
    param->set_repetition_filter(
          m_repetitions ? ossia::repetition_filter::OFF
                              : ossia::repetition_filter::ON);
  }
}



void parameter_base::set_enable()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_disabled(*node, !m_enable);
  }
}

void parameter_base::set_type()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::try_setup_parameter(m_type->s_name, *node);
  }
}

void parameter_base::set_rate()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_refresh_rate(*node,m_rate);
  }
}

void parameter_base::set_minmax(){
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

void parameter_base::set_range()
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

void parameter_base::set_bounding_mode()
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
    else if (bounding_mode == "both")
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
      pd_error(this, "unknown clip mode: %s", bounding_mode.c_str());
    }
  }
}

void parameter_base::set_default()
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

void parameter_base::get_range(parameter_base*x)
{
  // TODO how to get min/max/range from domain ?
  outlet_anything(x->m_dumpout, gensym("range"), x->m_range_size, x->m_range);
}

void parameter_base::get_min(parameter_base*x)
{
  outlet_anything(x->m_dumpout, gensym("min"), x->m_min_size, x->m_min);
}

void parameter_base::get_max(parameter_base*x)
{
  outlet_anything(x->m_dumpout, gensym("max"), x->m_max_size, x->m_max);
}

void parameter_base::get_bounding_mode(parameter_base*x)
{
  // assume all matchers have the same bounding_mode
  ossia::pd::t_matcher& m = x->m_matchers[0];
  ossia::net::node_base* node = m.get_node();
  ossia::net::parameter_base* param = node->get_parameter();

  x->m_bounding_mode = bounding_mode2symbol(param->get_bounding());
  t_atom a;
  SETSYMBOL(&a,x->m_bounding_mode);
  outlet_anything(x->m_dumpout, gensym("clip"), 1, &a);
}

void parameter_base::get_default(parameter_base*x)
{
  // assume all matchers have the same default value
  ossia::pd::t_matcher& m = x->m_matchers[0];
  ossia::net::node_base* node = m.get_node();
  ossia::net::parameter_base* param = node->get_parameter();

  auto def_val = ossia::net::get_default_value(*node);

  if ( def_val ){
    std::vector<t_atom> va;
    value2atom vm{va};
    ossia::value v = *def_val;
    v.apply(vm);

    x->m_default_size = va.size() > OSSIA_PD_MAX_ATTR_SIZE ? OSSIA_PD_MAX_ATTR_SIZE : va.size();

    for (int i=0; i < x->m_default_size; i++ )
      x->m_default[i] = va[i];
  } else {
    x->m_default_size = 0;
  }

  outlet_anything(x->m_dumpout, gensym("default"),
                  x->m_default_size, x->m_default);
}

void parameter_base::get_type(parameter_base*x)
{

  // assume all matchers have the same type
  ossia::pd::t_matcher& m = x->m_matchers[0];
  ossia::net::node_base* node = m.get_node();
  ossia::net::parameter_base* param = node->get_parameter();

  x->m_type = val_type2symbol(param->get_value_type());

  t_atom a;
  SETSYMBOL(&a,x->m_type);
  outlet_anything(x->m_dumpout, gensym("type"), 1, &a);
}

void parameter_base::get_access_mode(parameter_base*x)
{
  // assume all matchers have the same bounding_mode
  ossia::pd::t_matcher& m = x->m_matchers[0];
  ossia::net::node_base* node = m.get_node();
  ossia::net::parameter_base* param = node->get_parameter();

  x->m_access_mode = access_mode2symbol(param->get_access());

  t_atom a;
  SETSYMBOL(&a, x->m_access_mode);
  outlet_anything(x->m_dumpout, gensym("mode"), 1, &a);
}

void parameter_base::get_repetition_filter(parameter_base*x)
{
  // assume all matchers have the same bounding_mode
  ossia::pd::t_matcher& m = x->m_matchers[0];
  ossia::net::node_base* node = m.get_node();
  ossia::net::parameter_base* param = node->get_parameter();

  x->m_repetitions = !param->get_repetition_filter();

  t_atom a;
  SETFLOAT(&a, x->m_repetitions);
  outlet_anything(x->m_dumpout, gensym("repetitions"), 1, &a);
}

void parameter_base::get_enable(parameter_base*x)
{
  // assume all matchers have the same bounding_mode
  ossia::pd::t_matcher& m = x->m_matchers[0];
  ossia::net::node_base* node = m.get_node();
  ossia::net::parameter_base* param = node->get_parameter();

  x->m_enable = !param->get_disabled();

  t_atom a;
  SETFLOAT(&a,x->m_enable);
  outlet_anything(x->m_dumpout, gensym("enable"), 1, &a);
}

void parameter_base::push(parameter_base* x, t_symbol* s, int argc, t_atom* argv)
{
  ossia::net::node_base* node;

  if (!x->m_mute)
  {
    for (auto& m : x->m_matchers)
    {
      node = m.get_node();
      auto parent = m.get_parent();
      auto param = node->get_parameter();

      if (node && param)
      {
        if (argc == 1)
        {
          ossia::value v;
          // convert one element array to single element
          if (argv->a_type == A_SYMBOL)
            v = ossia::value(std::string(atom_getsymbol(argv)->s_name));
          else if (argv->a_type == A_FLOAT)
            v = ossia::value(atom_getfloat(argv));

          ossia::value converted;

          ossia::pd::parameter_base* x = (ossia::pd::parameter_base*) parent;

          if ( x->m_ounit != ossia::none )
          {
            auto src_unit = *x->m_ounit;
            auto dst_unit = param->get_unit();

            converted = ossia::convert(v, src_unit, dst_unit);
          } else
            converted = v;

          node->get_parameter()->push_value(converted);
        }
        else
        {
          std::vector<ossia::value> list;

          if ( s && s != gensym("list") )
            list.push_back(std::string(s->s_name));

          for (; argc > 0; argc--, argv++)
          {
            if (argv->a_type == A_SYMBOL)
              list.push_back(std::string(atom_getsymbol(argv)->s_name));
            else if (argv->a_type == A_FLOAT)
              list.push_back(atom_getfloat(argv));
            else
              pd_error(x, "value type not handled");
          }

          ossia::pd::parameter_base* xparam = (ossia::pd::parameter_base*) x;

          if ( xparam->m_ounit != ossia::none )
          {
            auto src_unit = *xparam->m_ounit;
            auto dst_unit = param->get_unit();

            auto converted = ossia::convert(list, src_unit, dst_unit);
            node->get_parameter()->push_value(converted);
          } else {
            node->get_parameter()->push_value(list);
          }
        }
      }
    }
  }
}

void parameter_base::bang(parameter_base* x)
{
  for (auto& matcher : x->m_matchers)
  {
    matcher.enqueue_value(matcher.get_node()->get_parameter()->value());
    matcher.output_value();
  }
}

void parameter_base::output_value(parameter_base* x)
{
  for (auto& m : x->m_matchers)
  {
    m.output_value();
  }
  clock_delay(x->m_poll_clock, x->m_rate);
}

void parameter_base::push_default_value(parameter_base* x)
{
  ossia::net::node_base* node;

  if (!x->m_mute)
  {
    for (auto& m : x->m_matchers)
    {
      node = m.get_node();
      auto parent = m.get_parent();
      auto param = node->get_parameter();

      auto def_val = ossia::net::get_default_value(*node);
      if (def_val)
        param->push_value(*def_val);
    }
  }
}

void parameter_base::declare_attributes(t_eclass* c)
{
  object_base :: declare_attributes(c);

  eclass_addmethod(c, (method) push,               "anything", A_GIMME, 0);
  eclass_addmethod(c, (method) bang,               "bang",     A_NULL,  0);
  eclass_addmethod(c, (method) push_default_value, "reset",    A_NULL,  0);


  CLASS_ATTR_INT(         c, "enable",            0, parameter_base, m_enable);
  eclass_addmethod(c, (method) parameter_base::get_enable,            "getenable",            A_NULL, 0);

  CLASS_ATTR_ATOM_VARSIZE(c, "default",           0, parameter_base, m_default, m_default_size, OSSIA_PD_MAX_ATTR_SIZE);
  eclass_addmethod(c, (method) parameter_base::get_default,           "getdefault",           A_NULL, 0);

  CLASS_ATTR_ATOM_VARSIZE(c, "range",             0, parameter_base, m_range,   m_range_size,   OSSIA_PD_MAX_ATTR_SIZE);
  eclass_addmethod(c, (method) parameter_base::get_range,             "getrange",             A_NULL, 0);

  CLASS_ATTR_ATOM_VARSIZE(c, "min",               0, parameter_base, m_min,     m_min_size,     OSSIA_PD_MAX_ATTR_SIZE);
  eclass_addmethod(c, (method) parameter_base::get_min,               "getmin",               A_NULL, 0);

  CLASS_ATTR_ATOM_VARSIZE(c, "max",               0, parameter_base, m_max,     m_max_size,     OSSIA_PD_MAX_ATTR_SIZE);
  eclass_addmethod(c, (method) parameter_base::get_max,               "getmax",               A_NULL, 0);

  CLASS_ATTR_SYMBOL(      c, "clip", 0, parameter_base, m_bounding_mode);
  eclass_addmethod(c, (method) parameter_base::get_bounding_mode,     "getclip",     A_NULL, 0);

  CLASS_ATTR_SYMBOL(c, "type", 0, parameter_base, m_type);
  eclass_addmethod(c, (method) parameter_base::get_type,              "gettype",              A_NULL, 0);

  CLASS_ATTR_SYMBOL(c, "mode", 0, parameter_base, m_access_mode);
  eclass_addmethod(c, (method) parameter_base::get_access_mode,       "getmode",       A_NULL, 0);

  CLASS_ATTR_FLOAT       (c, "repetitions", 0, parameter_base, m_repetitions);
  eclass_addmethod(c, (method) parameter_base::get_repetition_filter, "getrepetitions", A_NULL, 0);

  CLASS_ATTR_INT   (c, "mute",          0, remote, m_mute);

}

} // namespace pd
} // namespace ossia
