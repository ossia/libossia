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
}

void parameter_base::update_attribute(parameter_base* x, ossia::string_view attribute, const ossia::net::node_base* node)
{
  auto matchers = make_matchers_vector(x,node);

  if ( attribute == ossia::net::text_refresh_rate() ){
    get_rate(x, matchers);
  } else if ( attribute == ossia::net::text_muted() ){
    get_mute(x, matchers);
  } else if ( attribute == ossia::net::text_unit() ){
    get_unit(x, matchers);
  } else if ( attribute == ossia::net::text_value_type() ){
    get_type(x, matchers);
  } else if ( attribute == ossia::net::text_domain() ){
    get_domain(x, matchers);
  } else if ( attribute == ossia::net::text_access_mode() ){
    get_access_mode(x, matchers);
  } else if ( attribute == ossia::net::text_bounding_mode() ){
    get_bounding_mode(x, matchers);
  } else if ( attribute == ossia::net::text_disabled() ){
    get_enable(x, matchers);
  } else if ( attribute == ossia::net::text_repetition_filter() ){
    get_repetition_filter(x, matchers);
  } else if ( attribute == ossia::net::text_default_value() ) {
    get_default(x, matchers);
  } else {
    object_base::update_attribute((node_base*)x, attribute, node);
  }
}

void parameter_base::set_access_mode()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    auto param = node->get_parameter();

    std::string access_mode = m_access_mode->s_name;
    ossia::transform(access_mode, access_mode.begin(), ::tolower);
    m_access_mode = gensym(access_mode.c_str());

    param->set_access(symbol2access_mode(m_access_mode));
  }
}

void parameter_base::set_repetition_filter()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    auto param = node->get_parameter();
    param->set_repetition_filter(
          m_repetitions ? ossia::repetition_filter::OFF
                              : ossia::repetition_filter::ON);
  }
}



void parameter_base::set_enable()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_disabled(*node, !m_enable);
  }
}

void parameter_base::set_type()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::try_setup_parameter(m_type->s_name, *node);
  }
}

void parameter_base::set_rate()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_refresh_rate(*node,m_rate);
  }
}

void parameter_base::set_minmax()
{
  std::vector<ossia::value> min = attribute2value(m_min, m_min_size);
  std::vector<ossia::value> max = attribute2value(m_max, m_max_size);

  if(min.empty() && max.empty())
  {
    return;
  }

  min.resize(OSSIA_PD_MAX_ATTR_SIZE);
  max.resize(OSSIA_PD_MAX_ATTR_SIZE);

  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    param->set_domain(make_domain_from_minmax(min, max, param->get_value_type()));
  }
}

void parameter_base::set_range()
{
  if(m_range_size == 0)
    return;
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
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
      float fmin = m_range[0].a_w.w_float;
      float fmax = m_range[1].a_w.w_float;
      std::vector<ossia::value> min{};
      std::vector<ossia::value> max{};

      switch(param->get_value_type())
      {
        case ossia::val_type::FLOAT:
        case ossia::val_type::INT:
        case ossia::val_type::CHAR:
          min={fmin};
          max={fmax};
          break;
        case ossia::val_type::VEC2F:
          min={fmin,fmin};
          max={fmax,fmax};
          break;
        case ossia::val_type::VEC3F:
          min={fmin,fmin,fmin};
          max={fmax,fmax,fmax};
          break;
        case ossia::val_type::VEC4F:
          min={fmin,fmin,fmin,fmin};
          max={fmax,fmax,fmax,fmax};
          break;
        case ossia::val_type::LIST:
          min.resize(OSSIA_PD_MAX_ATTR_SIZE);
          max.resize(OSSIA_PD_MAX_ATTR_SIZE);
          ossia::fill(min,fmin);
          ossia::fill(max,fmax);
          break;
        default:
          ;
      }
      auto domain = make_domain_from_minmax({min}, max, param->get_value_type());
      param->set_domain(domain);
    }
    else
    {
      param->set_domain({});
    }
  }
}

void parameter_base::set_bounding_mode()
{
  std::string bounding_mode = m_bounding_mode->s_name;
  ossia::transform(bounding_mode, bounding_mode.begin(), ::tolower);
  m_bounding_mode = gensym(bounding_mode.c_str());

  auto mode = symbol2bounding_mode(m_bounding_mode);

  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    param->set_bounding(mode);
  }
}

void parameter_base::set_default()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
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

void parameter_base::set_unit()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
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

void parameter_base::set_mute()
{
  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_muted(*node,m_mute);
  }
}

void parameter_base::get_domain(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    auto domain = ossia::net::get_domain(*m->get_node());
    if (domain)
    {
      domain_visitor dv;
      dv.x = x;
      ossia::apply(dv, domain.v);
    } else {
      x->m_range_size = 0;
      x->m_min_size = 0;
      x->m_max_size = 0;
    }
    outlet_anything(x->m_dumpout, gensym("range"), x->m_range_size, x->m_range);
    outlet_anything(x->m_dumpout, gensym("min"), x->m_min_size, x->m_min);
    outlet_anything(x->m_dumpout, gensym("max"), x->m_max_size, x->m_max);
  }
}

void parameter_base::get_bounding_mode(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_bounding_mode = bounding_mode2symbol(param->get_bounding());
    t_atom a;
    SETSYMBOL(&a,x->m_bounding_mode);
    outlet_anything(x->m_dumpout, gensym("clip"), 1, &a);
  }
}

void parameter_base::get_default(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    auto def_val = ossia::net::get_default_value(*m->get_node());

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
}

void parameter_base::get_type(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_type = val_type2symbol(param->get_value_type());

    t_atom a;
    SETSYMBOL(&a,x->m_type);
    outlet_anything(x->m_dumpout, gensym("type"), 1, &a);
  }
}

void parameter_base::get_access_mode(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_access_mode = access_mode2symbol(param->get_access());

    t_atom a;
    SETSYMBOL(&a, x->m_access_mode);
    outlet_anything(x->m_dumpout, gensym("mode"), 1, &a);
  }
}

void parameter_base::get_repetition_filter(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_repetitions = !param->get_repetition_filter();

    t_atom a;
    SETFLOAT(&a, x->m_repetitions);
    outlet_anything(x->m_dumpout, gensym("repetitions"), 1, &a);
  }
}

void parameter_base::get_enable(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    auto param = m->get_node()->get_parameter();
    x->m_enable = !param->get_disabled();

    t_atom a;
    SETFLOAT(&a,x->m_enable);
    outlet_anything(x->m_dumpout, gensym("enable"), 1, &a);
  }
}

void parameter_base::get_unit(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    std::string unit = ossia::get_pretty_unit_text(param->get_unit());
    x->m_unit = gensym(unit.c_str());

    t_atom a;
    SETSYMBOL(&a, x->m_unit);
    outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
  }
}

void parameter_base::get_mute(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_mute = param->get_muted();

    t_atom a;
    SETFLOAT(&a, x->m_mute);
    outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
  }
}

void parameter_base::get_rate(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    auto rate = ossia::net::get_refresh_rate(*m->get_node());

    if (rate)
    {
      x->m_rate = *rate;

      t_atom a;
      SETFLOAT(&a, x->m_rate);
      outlet_anything(x->m_dumpout, gensym("rate"), 1, &a);
    }
  }
}

template<std::size_t N>
ossia::optional<std::array<float, N>> to_array(t_atom* argv)
{
  std::array<float, N> arr;
  for(std::size_t i = 0; i < N; i++)
  {
    switch(argv[i].a_type)
    {
      case A_FLOAT:
        arr[i] = atom_getfloat(&argv[i]);
        break;
      default:
        return ossia::none;
    }
  }
  return arr;
}

void convert_or_push(parameter_base* x, ossia::value&& v, bool set_flag = false)
{
  for (auto m : x->m_node_selection)
  {
    auto node = m->get_node();
    auto param = node->get_parameter();
    auto xparam = (parameter_base*)m->get_owner();

    if ( xparam->m_ounit != ossia::none )
    {
      const auto& src_unit = *xparam->m_ounit;
      const auto& dst_unit = param->get_unit();

      auto converted = ossia::convert(v, src_unit, dst_unit);
      if (set_flag) m->m_set_pool.push_back(converted);
      param->push_value(converted);
    }
    else
    {
      if (set_flag) m->m_set_pool.push_back(v);
      param->push_value(v);
    }
  }
}

void just_push(parameter_base* x, ossia::value&& v, bool set_flag = false)
{
  for (auto m : x->m_node_selection)
  {
    auto node = m->get_node();
    auto param = node->get_parameter();
    if(set_flag) m->m_set_pool.push_back(v);
    param->push_value(v);
  }
}


void parameter_base::push(parameter_base* x, t_symbol* s, int argc, t_atom* argv)
{
  ossia::net::node_base* node;
  bool set_flag = false;
  if (s && s == gensym("set"))
    set_flag = true;

  if (!x->m_mute)
  {
    if (argc == 0 && s)
    {
      just_push(x, std::string(s->s_name), set_flag);
    }
    else if (argc == 1)
    {
      ossia::value v;
      // convert one element array to single element
      switch(argv->a_type)
      {
        case A_SYMBOL:
          just_push(x, std::string(atom_getsymbol(argv)->s_name), set_flag);
          break;
        case A_FLOAT:
          convert_or_push(x, atom_getfloat(argv), set_flag);
          break;
        default:
          ;
      }
    }
    else
    {
      std::vector<ossia::value> list;
      list.reserve(argc+1);
      if ( s && s != gensym("list") && s != gensym("set"))
      {
        list.push_back(std::string(s->s_name));
      }

      switch(argc)
      {
        case 2: if(auto arr = to_array<2>(argv)) { convert_or_push(x, *arr, set_flag); return; } break;
        case 3: if(auto arr = to_array<3>(argv)) { convert_or_push(x, *arr, set_flag); return; } break;
        case 4: if(auto arr = to_array<4>(argv)) { convert_or_push(x, *arr, set_flag); return; } break;
      }

      for (; argc > 0; argc--, argv++)
      {
        switch (argv->a_type)
        {
          case A_SYMBOL:
            list.push_back(std::string(atom_getsymbol(argv)->s_name));
            break;
          case A_FLOAT:
            list.push_back(atom_getfloat(argv));
            break;
          default:
            pd_error(x, "value type not handled");
        }
      }

      ossia::pd::parameter_base* xparam = static_cast<ossia::pd::parameter_base*>(x);

      convert_or_push(x, std::move(list), set_flag);
    }
  }
}

void parameter_base::bang(parameter_base* x)
{
  for (auto m : x->m_node_selection)
  {
    auto param = m->get_node()->get_parameter();

    if (param->get_value_type() == ossia::val_type::IMPULSE)
      param->push_value(ossia::impulse{});
    else
    {
      m->enqueue_value(param->value());
      m->output_value();
    }
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
    for (auto m : x->m_node_selection)
    {
      node = m->get_node();
      auto param = node->get_parameter();

      auto def_val = ossia::net::get_default_value(*node);
      if (def_val)
        param->push_value(*def_val);
    }
  }
}

void parameter_base::get_mess_cb(parameter_base* x, t_symbol* s)
{
  if ( s == gensym("enable") )
    parameter_base::get_enable(x,x->m_node_selection);
  else if ( s == gensym("default") )
    parameter_base::get_default(x,x->m_node_selection);
  else if ( s == gensym("range") || s == gensym("min") || s == gensym("max") )
    parameter_base::get_domain(x,x->m_node_selection);
  else if ( s == gensym("clip") )
    parameter_base::get_bounding_mode(x,x->m_node_selection);
  else if ( s == gensym("type") )
    parameter_base::get_type(x,x->m_node_selection);
  else if ( s == gensym("mode") )
    parameter_base::get_access_mode(x,x->m_node_selection);
  else if ( s == gensym("repetitions") )
    parameter_base::get_repetition_filter(x,x->m_node_selection);
  else if ( s == gensym("mute") )
    parameter_base::get_mute(x,x->m_node_selection);
  else if ( s == gensym("unit") )
    parameter_base::get_unit(x,x->m_node_selection);
  else if ( s == gensym("rate") )
    parameter_base::get_rate(x,x->m_node_selection);
  else
    object_base::get_mess_cb(x,s);
}

void parameter_base::class_setup(t_eclass* c)
{
  object_base :: class_setup(c);

  if (c != ossia_pd::attribute_class)
  {
    eclass_addmethod(c, (method) push,               "anything", A_GIMME, 0);
    eclass_addmethod(c, (method) push,               "set",      A_GIMME, 0);
    eclass_addmethod(c, (method) bang,               "bang",     A_NULL,  0);
    eclass_addmethod(c, (method) push_default_value, "reset",    A_NULL,  0);
  }

  CLASS_ATTR_INT(         c, "enable",      0, parameter_base, m_enable);
  CLASS_ATTR_ATOM_VARSIZE(c, "default",     0, parameter_base, m_default, m_default_size, OSSIA_PD_MAX_ATTR_SIZE);
  CLASS_ATTR_ATOM_VARSIZE(c, "range",       0, parameter_base, m_range,   m_range_size,   OSSIA_PD_MAX_ATTR_SIZE);
  CLASS_ATTR_ATOM_VARSIZE(c, "min",         0, parameter_base, m_min,     m_min_size,     OSSIA_PD_MAX_ATTR_SIZE);
  CLASS_ATTR_ATOM_VARSIZE(c, "max",         0, parameter_base, m_max,     m_max_size,     OSSIA_PD_MAX_ATTR_SIZE);
  CLASS_ATTR_SYMBOL      (c, "clip",        0, parameter_base, m_bounding_mode);
  CLASS_ATTR_SYMBOL      (c, "type",        0, parameter_base, m_type);
  CLASS_ATTR_SYMBOL      (c, "mode",        0, parameter_base, m_access_mode);
  CLASS_ATTR_FLOAT       (c, "repetitions", 0, parameter_base, m_repetitions);
  CLASS_ATTR_INT         (c, "mute",        0, parameter_base, m_mute);
  CLASS_ATTR_SYMBOL      (c, "unit",        0, parameter_base, m_unit);
  CLASS_ATTR_FLOAT       (c, "rate",        0, parameter_base, m_rate);

  // TODO use those to tweak attributes
  // CLASS_ATTR_FILTER_CLIP
  // CLASS_ATTR_STEP
}

} // namespace pd
} // namespace ossia
