// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "parameter_base.hpp"

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia-pd/src/utils.hpp>

#include <sstream>

namespace ossia {
namespace pd {

parameter_base::parameter_base(t_eclass* x)
  : t_object_base{x}
{ }

void parameter_base::update_attribute(parameter_base* x, ossia::string_view attribute)
{
  if ( attribute == ossia::net::text_value_type() ){
    logpost(x,2,"update type attribute");
  } else if ( attribute == ossia::net::text_domain() ){
    logpost(x,2,"update domain attribute");
  } else if ( attribute == ossia::net::text_access_mode() ){
    logpost(x,2,"update acces_mode attribute");
  } else if ( attribute == ossia::net::text_bounding_mode() ){
    logpost(x,2,"update bounding_mode attribute");
  } else if ( attribute == ossia::net::text_muted() ){
    logpost(x,2,"update mute attribute");
  } else if ( attribute == ossia::net::text_disabled() ){
    logpost(x,2,"update enable attribute");
  } else if ( attribute == ossia::net::text_repetition_filter() ){
    logpost(x,2,"update repetition_filter attribute");
  } else {
    logpost(x,2,"no attribute %s", attribute);
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

void parameter_base::set_repetition_filter()
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

void parameter_base::set_description()
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

void parameter_base::set_tags()
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

void parameter_base::set_priority()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_priority(*node, m_priority);
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
    ossia::net::parameter_base* param = node->get_parameter();
    param->set_value_type(symbol2val_type(m_type));
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

void parameter_base::set_hidden()
{
  for (t_matcher& m : m_matchers)
  {
    ossia::net::node_base* node = m.get_node();
    ossia::net::set_hidden(*node, m_hidden);
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
  t_atom a;
  SETSYMBOL(&a,x->m_bounding_mode);
  outlet_anything(x->m_dumpout, gensym("bounding_mode"), 1, &a);
}

void parameter_base::get_default(parameter_base*x)
{
  outlet_anything(x->m_dumpout, gensym("default"),
                  x->m_default_size, x->m_default);
}

void parameter_base::get_type(parameter_base*x)
{
  t_atom a;
  SETSYMBOL(&a,x->m_type);
  outlet_anything(x->m_dumpout, gensym("type"), 1, &a);
}

void parameter_base::get_hidden(parameter_base*x)
{
  t_atom a;
  SETFLOAT(&a, x->m_hidden);
  outlet_anything(x->m_dumpout, gensym("hidden"), 1, &a);
}

void parameter_base::get_priority(parameter_base*x)
{
  t_atom a;
  SETFLOAT(&a, x->m_priority);
  outlet_anything(x->m_dumpout, gensym("priority"), 1, &a);
}

void parameter_base::get_access_mode(parameter_base*x)
{
  t_atom a;
  SETSYMBOL(&a, x->m_access_mode);
  outlet_anything(x->m_dumpout, gensym("access_mode"), 1, &a);
}

void parameter_base::get_repetition_filter(parameter_base*x)
{
  t_atom a;
  SETFLOAT(&a, x->m_repetition_filter);
  outlet_anything(x->m_dumpout, gensym("repetition_filter"), 1, &a);
}

void parameter_base::get_tags(parameter_base*x)
{
  outlet_anything(x->m_dumpout, gensym("tags"),
                  x->m_tags_size, x->m_tags);
}

void parameter_base::get_description(parameter_base*x)
{
  outlet_anything(x->m_dumpout, gensym("description"),
                  x->m_description_size, x->m_description);
}

void parameter_base::get_enable(parameter_base*x)
{
  t_atom a;
  SETFLOAT(&a,x->m_enable);
  outlet_anything(x->m_dumpout, gensym("enable"), 1, &a);
}

} // namespace pd
} // namespace ossia
