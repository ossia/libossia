// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/complex_type.hpp>

#include <ossia-max/src/parameter_base.hpp>
#include <ossia-max/src/utils.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <algorithm>
#include <sstream>

namespace ossia
{
namespace max_binding
{

void parameter_base::update_attribute(
    parameter_base* x, ossia::string_view attribute, const ossia::net::node_base* node)
{
  auto matchers = make_matchers_vector(x, node);

  if(matchers.empty())
    return;

  if(attribute == ossia::net::text_refresh_rate())
  {
    get_rate(x, matchers);
  }
  else if(attribute == ossia::net::text_value_type())
  {
    get_type(x, matchers);
  }
  else if(attribute == ossia::net::text_domain())
  {
    get_domain(x, matchers);
  }
  else if(attribute == ossia::net::text_access_mode())
  {
    get_access_mode(x, matchers);
  }
  else if(attribute == ossia::net::text_bounding_mode())
  {
    get_bounding_mode(x, matchers);
  }
  else if(attribute == ossia::net::text_disabled())
  {
    get_enable(x, matchers);
  }
  else if(attribute == ossia::net::text_repetition_filter())
  {
    get_repetition_filter(x, matchers);
  }
  else if(attribute == ossia::net::text_critical())
  {
    get_critical(x, matchers);
  }
  else if(attribute == ossia::net::text_default_value())
  {
    get_default(x, matchers);
  }
  else if(attribute == ossia::net::text_extended_type())
  {
    get_type(x, matchers);
    get_unit(x, matchers);
  }
  else if(attribute == ossia::net::text_muted())
  {
    get_mute(x, matchers);
  }
  else if(attribute == ossia::net::text_unit())
  {
    get_unit(x, matchers);
  }
  else
  {
    object_base::update_attribute((node_base*)x, attribute, node);
  }
}

t_max_err parameter_base::notify(
    parameter_base* x, t_symbol* s, t_symbol* msg, void* sender, void* data)
{
  t_symbol* attrname;

  if(!x->m_lock && msg == gensym("attr_modified"))
  {
    attrname = (t_symbol*)object_method((t_object*)data, gensym("getname"));

    if(attrname == _sym_unit)
      x->set_unit();
    else if(attrname == gensym("mode"))
      x->set_access_mode();
    else if(attrname == gensym("repetitions"))
      x->set_repetition_filter();
    else if(attrname == gensym("critical"))
      x->set_critical();
    else if(attrname == gensym("enable"))
      x->set_enable();
    else if(attrname == gensym("type"))
      x->set_type();
    else if(attrname == gensym("rate"))
      x->set_rate();
    else if(attrname == gensym("range"))
      x->set_range();
    else if(attrname == gensym("clip"))
      x->set_bounding_mode();
    else if(attrname == _sym_min || attrname == _sym_max)
      x->set_minmax();
    else if(attrname == gensym("default"))
      x->set_default();
    else
      object_base::notify(x, s, msg, sender, data);
  }
  return 0;
}

void parameter_base::set_access_mode()
{
  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      auto param = node->get_parameter();

      std::string access_mode_str = m_access_mode->s_name;
      boost::algorithm::to_lower(access_mode_str);
      m_access_mode = gensym(access_mode_str.c_str());

      auto access_mode = symbol2access_mode(m_access_mode);
      param->set_access(access_mode);
      ossia::net::set_access_mode(*node, access_mode);
    }
  }
}

void parameter_base::set_repetition_filter()
{
  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      auto param = node->get_parameter();
      param->set_repetition_filter(
          m_repetitions ? ossia::repetition_filter::OFF : ossia::repetition_filter::ON);
    }
  }
}

void parameter_base::set_critical()
{
  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      auto param = node->get_parameter();
      param->set_critical(m_critical);
    }
  }
}

void parameter_base::set_enable()
{
  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      ossia::net::set_disabled(*node, !m_enable);
    }
  }
}

void parameter_base::set_type()
{
  for(auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::try_setup_parameter(m_type->s_name, *node);
  }
}

void parameter_base::set_rate()
{
  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      ossia::net::set_refresh_rate(*node, m_rate);
    }
  }
}

void parameter_base::set_minmax()
{
  std::vector<ossia::value> _min = attribute2value(m_min, m_min_size);
  std::vector<ossia::value> _max = attribute2value(m_max, m_max_size);

  if(_min.empty() && _max.empty())
  {
    return;
  }

  for(matcher* m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      ossia::net::parameter_base* param = node->get_parameter();

      auto min = _min;
      auto max = _max;

      switch(param->get_value_type())
      {
        case ossia::val_type::BOOL:
        case ossia::val_type::INT:
        case ossia::val_type::FLOAT:
          min.resize(1);
          max.resize(1);
          break;
        case ossia::val_type::VEC2F:
          min.resize(2);
          max.resize(2);
          break;
        case ossia::val_type::VEC3F:
          min.resize(3);
          max.resize(3);
          break;
        case ossia::val_type::VEC4F:
          min.resize(4);
          max.resize(4);
          break;
        case ossia::val_type::LIST:
          min.resize(OSSIA_MAX_MAX_ATTR_SIZE);
          max.resize(OSSIA_MAX_MAX_ATTR_SIZE);
          break;
        case ossia::val_type::MAP:
          break;
        default:
          continue;
      }

      auto domain = make_domain_from_minmax({min}, max, param->get_value_type());
      param->set_domain(domain);
    }
  }
}

void parameter_base::set_unit()
{
  assert(m_otype != object_class::remote);
  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      ossia::net::parameter_base* param = node->get_parameter();

      if(m_unit != gensym(""))
      {
        ossia::unit_t unit = ossia::parse_pretty_unit(m_unit->s_name);
        if(unit)
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
}

void parameter_base::set_mute()
{
  for(matcher* m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      ossia::net::set_muted(*node, m_mute);
    }
  }
}

void parameter_base::push_default_value(parameter_base* x)
{
  ossia::net::node_base* node;

  if(!x->m_mute)
  {
    for(auto m : x->m_node_selection)
    {
      if(!m->is_zombie())
      {
        node = m->get_node();
        auto param = node->get_parameter();

        auto it = x->m_value_map.find(node->get_name());
        if(it != x->m_value_map.end())
        {
          x->push_parameter_value(param, it->second);
        }
        else
        {
          auto def_val = ossia::net::get_default_value(*node);
          if(def_val)
          {
            x->push_parameter_value(param, *def_val);
          }
        }
      }
    }
  }
}

void parameter_base::set_range()
{
  if(m_range_size == 0)
    return;

  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      ossia::net::parameter_base* param = node->get_parameter();

      if(param->get_value_type() == ossia::val_type::STRING)
      {
        std::vector<std::string> senum;
        for(int i = 0; i < m_range_size; i++)
        {
          if(m_range[i].a_type == A_SYM)
            senum.push_back(m_range[i].a_w.w_sym->s_name);
          else if(m_range[i].a_type == A_FLOAT)
          {
            senum.push_back(fmt::format("{}", m_range[i].a_w.w_float));
          }
          else if(m_range[i].a_type == A_LONG)
          {
            senum.push_back(fmt::format("{}", m_range[i].a_w.w_long));
          }
          else
            break;
        }
        param->set_domain(make_domain(senum));
      }
      else if(
          m_range_size > 1
          && (m_range[0].a_type == A_FLOAT || m_range[0].a_type == A_LONG)
          && (m_range[1].a_type == A_FLOAT || m_range[1].a_type == A_LONG))
      {
        float fmin = atom_getfloat(m_range);
        float fmax = atom_getfloat(m_range + 1);
        std::vector<ossia::value> min{};
        std::vector<ossia::value> max{};

        switch(param->get_value_type())
        {
          case ossia::val_type::FLOAT:
          case ossia::val_type::INT:
            min = {fmin};
            max = {fmax};
            break;
          case ossia::val_type::VEC2F:
            min = {fmin, fmin};
            max = {fmax, fmax};
            break;
          case ossia::val_type::VEC3F:
            min = {fmin, fmin, fmin};
            max = {fmax, fmax, fmax};
            break;
          case ossia::val_type::VEC4F:
            min = {fmin, fmin, fmin, fmin};
            max = {fmax, fmax, fmax, fmax};
            break;
          case ossia::val_type::LIST:
            // FIXME use something like MAX_ATTR_SIZE instead of arbitrary constant
            min.resize(OSSIA_MAX_MAX_ATTR_SIZE);
            max.resize(OSSIA_MAX_MAX_ATTR_SIZE);
            ossia::fill(min, fmin);
            ossia::fill(max, fmax);
            break;
          case ossia::val_type::MAP:
          default:
            break;
        }

        auto domain = make_domain_from_minmax(min, max, param->get_value_type());
        param->set_domain(domain);
      }
      else
      {
        param->set_domain({});
      }
    }
  }
}

void parameter_base::set_bounding_mode()
{
  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      ossia::net::parameter_base* param = node->get_parameter();

      std::string bounding_mode = m_bounding_mode->s_name;
      boost::algorithm::to_lower(bounding_mode);
      m_bounding_mode = gensym(bounding_mode.c_str());

      if(bounding_mode == "free")
        param->set_bounding(ossia::bounding_mode::FREE);
      else if(bounding_mode == "both")
        param->set_bounding(ossia::bounding_mode::CLIP);
      else if(bounding_mode == "wrap")
        param->set_bounding(ossia::bounding_mode::WRAP);
      else if(bounding_mode == "fold")
        param->set_bounding(ossia::bounding_mode::FOLD);
      else if(bounding_mode == "low")
        param->set_bounding(ossia::bounding_mode::CLAMP_LOW);
      else if(bounding_mode == "high")
        param->set_bounding(ossia::bounding_mode::CLAMP_HIGH);
      else
      {
        object_error((t_object*)this, "unknown clip mode: %s", bounding_mode.c_str());
      }
    }
  }
}

template <std::size_t N>
auto to_vec(t_atom* m_default)
{
  std::array<float, N> arr;
  for(int i = 0; i < N; i++)
  {
    switch(m_default[i].a_type)
    {
      case A_FLOAT:
        arr[i] = m_default[i].a_w.w_float;
        break;
      case A_LONG:
        arr[i] = (float)m_default[i].a_w.w_long;
        break;
      default:
        arr[i] = 0.f;
        break;
    }
  }
  return arr;
}

void parameter_base::set_default()
{
  for(auto m : m_node_selection)
  {
    if(!m->is_zombie())
    {
      ossia::net::node_base* node = m->get_node();
      ossia::net::parameter_base* param = node->get_parameter();

      switch(param->get_value_type())
      {
        case ossia::val_type::VEC4F:
          if(m_default_size >= 4)
            ossia::net::set_default_value(*node, max_binding::to_vec<4>(m_default));
          break;
        case ossia::val_type::VEC3F:
          if(m_default_size >= 3)
            ossia::net::set_default_value(*node, to_vec<3>(m_default));
          break;
        case ossia::val_type::VEC2F:
          if(m_default_size >= 2)
            ossia::net::set_default_value(*node, to_vec<2>(m_default));
          break;
        case ossia::val_type::FLOAT:
        case ossia::val_type::INT:
        case ossia::val_type::BOOL: {
          if(m_default[0].a_type == A_FLOAT)
          {
            ossia::net::set_default_value(*node, m_default[0].a_w.w_float);
          }
          else if(m_default[0].a_type == A_LONG)
          {
            ossia::net::set_default_value(*node, (int)m_default[0].a_w.w_long);
          }
          break;
        }
        case ossia::val_type::STRING: {
          if(m_default[0].a_type == A_SYM)
          {
            ossia::net::set_default_value(*node, m_default[0].a_w.w_sym->s_name);
          }
          break;
        }
        case ossia::val_type::LIST: {
          if(m_default_size > 0)
          {
            auto def = attribute2value(m_default, m_default_size);
            ossia::net::set_default_value(*node, def);
          }
          break;
        }
        case ossia::val_type::MAP:
        default:
          break;
      }
    }
  }
}

void parameter_base::get_mess_cb(parameter_base* x, t_symbol* s)
{
  if(s == gensym("enable"))
    parameter_base::get_enable(x, x->m_node_selection);
  else if(s == gensym("default"))
    parameter_base::get_default(x, x->m_node_selection);
  else if(s == gensym("range") || s == _sym_min || s == _sym_max)
    parameter_base::get_domain(x, x->m_node_selection);
  else if(s == gensym("clip"))
    parameter_base::get_bounding_mode(x, x->m_node_selection);
  else if(s == gensym("type"))
    parameter_base::get_type(x, x->m_node_selection);
  else if(s == gensym("mode"))
    parameter_base::get_access_mode(x, x->m_node_selection);
  else if(s == gensym("repetitions"))
    parameter_base::get_repetition_filter(x, x->m_node_selection);
  else if(s == gensym("critical"))
    parameter_base::get_critical(x, x->m_node_selection);
  else if(s == _sym_mute)
    parameter_base::get_mute(x, x->m_node_selection);
  else if(s == _sym_unit)
    parameter_base::get_unit(x, x->m_node_selection);
  else if(s == gensym("rate"))
    parameter_base::get_rate(x, x->m_node_selection);
  else
    object_base::get_mess_cb(x, s);
}

void parameter_base::get_domain(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    auto domain = ossia::net::get_domain(*m->get_node());
    if(domain)
    {
      domain_visitor dv;
      dv.x = x;
      ossia::apply(dv, domain.v);
    }
    else
    {
      x->m_range_size = 0;
      x->m_min_size = 0;
      x->m_max_size = 0;
    }

    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
    outlet_anything(x->m_dumpout, gensym("range"), x->m_range_size, x->m_range);
    outlet_anything(x->m_dumpout, _sym_min, x->m_min_size, x->m_min);
    outlet_anything(x->m_dumpout, _sym_max, x->m_max_size, x->m_max);
  }
  lock_and_touch(x, gensym("range"));
  lock_and_touch(x, _sym_min);
  lock_and_touch(x, _sym_max);
}

void parameter_base::get_bounding_mode(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    ossia::net::parameter_base* param = m->get_node()->get_parameter();
    if(param)
    {
      x->m_bounding_mode = bounding_mode2symbol(param->get_bounding());
      t_atom a;
      A_SETSYM(&a, x->m_bounding_mode);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("clip"), 1, &a);
    }
  }
  lock_and_touch(x, gensym("clip"));
}

void parameter_base::get_default(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    auto def_val = ossia::net::get_default_value(*m->get_node());

    if(def_val)
    {
      std::vector<t_atom> va;
      value2atom vm{va};
      ossia::value v = *def_val;
      v.apply(vm);

      x->m_default_size = va.size() > 512 ? 512 : va.size();

      for(int i = 0; i < x->m_default_size; i++)
        x->m_default[i] = va[i];
    }
    else
    {
      x->m_default_size = 0;
    }

    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
    outlet_anything(x->m_dumpout, gensym("default"), x->m_default_size, x->m_default);
  }
  lock_and_touch(x, gensym("default"));
}

void parameter_base::get_type(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    ossia::net::parameter_base* param = m->get_node()->get_parameter();
    if(param)
    {
      x->m_type = val_type2symbol(param->get_value_type());

      t_atom a;
      A_SETSYM(&a, x->m_type);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("type"), 1, &a);
    }
  }
  lock_and_touch(x, gensym("type"));
}

void parameter_base::get_access_mode(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    ossia::net::parameter_base* param = m->get_node()->get_parameter();
    if(param)
    {
      x->m_access_mode = access_mode2symbol(param->get_access());

      t_atom a;
      A_SETSYM(&a, x->m_access_mode);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("mode"), 1, &a);
    }
  }
  lock_and_touch(x, gensym("mode"));
}

void parameter_base::get_critical(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    if(param)
    {
      x->m_critical = param->get_critical();

      t_atom a;
      A_SETLONG(&a, x->m_critical);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("critical"), 1, &a);
    }
  }
  lock_and_touch(x, gensym("critical"));
}

void parameter_base::get_repetition_filter(
    parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    if(param)
    {
      x->m_repetitions = !param->get_repetition_filter();

      t_atom a;
      A_SETLONG(&a, x->m_repetitions);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("repetitions"), 1, &a);
    }
  }
  lock_and_touch(x, gensym("repetitions"));
}

void parameter_base::get_enable(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    auto param = m->get_node()->get_parameter();

    if(param)
    {
      x->m_enable = !param->get_disabled();

      t_atom a;
      A_SETLONG(&a, x->m_enable);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("enable"), 1, &a);
    }
  }
  lock_and_touch(x, gensym("enable"));
}

void parameter_base::get_unit(parameter_base* x, std::vector<matcher*> nodes)
{
  assert(x->m_otype != object_class::remote);
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    auto node = m->get_node();
    if(!node)
      continue;

    ossia::net::parameter_base* param = node->get_parameter();
    if(param)
    {
      std::string_view unit = ossia::get_pretty_unit_text(param->get_unit());
      x->m_unit = gensym(unit.data());

      t_atom a;
      A_SETSYM(&a, x->m_unit);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, _sym_unit, 1, &a);
    }
  }
  lock_and_touch(x, _sym_unit);
}

void parameter_base::get_mute(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    ossia::net::parameter_base* param = m->get_node()->get_parameter();
    if(param)
    {
      x->m_mute = param->get_muted();

      t_atom a;
      A_SETLONG(&a, x->m_mute);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, _sym_mute, 1, &a);
    }
  }
  lock_and_touch(x, _sym_mute);
}

void parameter_base::get_rate(parameter_base* x, std::vector<matcher*> nodes)
{
  for(auto m : nodes)
  {
    if(m->is_zombie() || m->is_dead())
      continue;

    auto rate = ossia::net::get_refresh_rate(*m->get_node());

    if(rate)
    {
      x->m_rate = *rate;

      t_atom a;
      A_SETFLOAT(&a, x->m_rate);

      outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());
      outlet_anything(x->m_dumpout, gensym("rate"), 1, &a);
    }
  }
  lock_and_touch(x, gensym("rate"));
}

template <std::size_t N>
std::optional<std::array<float, N>> to_array(t_atom* argv)
{
  std::array<float, N> arr;
  for(std::size_t i = 0; i < N; i++)
  {
    switch(argv[i].a_type)
    {
      case A_FLOAT:
        arr[i] = atom_getfloat(&argv[i]);
        break;
      case A_LONG:
        arr[i] = (float)atom_getlong(&argv[i]);
        break;
      default:
        return std::nullopt;
    }
  }
  return arr;
}

void convert_or_push(parameter_base* x, ossia::value&& v)
{
  for(auto m : x->m_node_selection)
  {
    if(m->is_zombie())
      continue;

    auto node = m->get_node();
    auto param = node->get_parameter();
    // FIXME we should have checked that before
    if(!param)
      return;

    auto xparam = (parameter_base*)m->get_owner();

    if(xparam->m_local_unit != std::nullopt)
    {
      const auto& src_unit = *xparam->m_local_unit;
      const auto& dst_unit = param->get_unit();

      auto converted = ossia::convert(v, src_unit, dst_unit);
      x->push_parameter_value(param, converted);
    }
    else
    {
      x->push_parameter_value(param, v);
    }
  }
}

void just_push(parameter_base* x, ossia::value&& v)
{
  for(auto& m : x->m_node_selection)
  {
    if(m->is_zombie())
      continue;

    auto node = m->get_node();
    auto param = node->get_parameter();
    x->push_parameter_value(param, v);
  }
}

void parameter_base::push(parameter_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if(x->m_mute)
    return;

  if(s && s == _sym_set)
    x->m_set_flag = true;

  // TODO use atom2value here

  if(argc == 0 && s)
  {
    just_push(x, std::string(s->s_name));
  }
  else if(argc == 1 && s && (s == _sym_float || s == _sym_list || (s == _sym_int)))
  {
    // convert one element array to single element
    switch(argv->a_type)
    {
      case A_SYM:
        just_push(x, std::string(atom_getsym(argv)->s_name));
        break;
      case A_FLOAT:
        convert_or_push(x, ossia::value(atom_getfloat(argv)));
        break;
      case A_LONG:
        convert_or_push(x, static_cast<int32_t>(atom_getlong(argv)));
        break;
      default:;
    }
  }
  else
  {

    std::vector<ossia::value> list;
    list.reserve(argc + 1);

    if(s && s != _sym_list && s != _sym_set)
    {
      list.push_back(std::string(s->s_name));
    }

    for(; argc > 0; argc--, argv++)
    {
      switch(argv->a_type)
      {
        case A_SYM:
          list.push_back(std::string(atom_getsym(argv)->s_name));
          break;
        case A_FLOAT:
          list.push_back(atom_getfloat(argv));
          break;
        case A_LONG:
          list.push_back(static_cast<long>(atom_getlong(argv)));
          break;
        default:
          object_error((t_object*)x, "value type not handled");
      }
    }
    if(list.size() == 1)
      convert_or_push(x, std::move(list[0]));
    else
      convert_or_push(x, std::move(list));
  }

  x->m_set_flag = false;
}

void parameter_base::push_one(parameter_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if(!s || argc < 2 || argv[0].a_type != A_SYM)
    return;

  auto target = atom_getsym(&argv[0])->s_name;
  object_base* owner{};
  ossia::net::node_base* node{};
  if(!x->m_mute)
  {
    for(const auto m : x->m_node_selection)
    {
      auto cur = m->get_node();
      auto addr = ossia::net::address_string_from_node(*cur);
      if(boost::algorithm::ends_with(addr, target))
      {
        node = cur;
        owner = m->get_owner();
        break;
      }
    }

    if(!node || !owner)
      return;

    auto param = node->get_parameter();
    if(!param)
      return;

    if(argc == 2)
    {
      auto arg = &argv[1];
      ossia::value v;
      // convert one element array to single element
      switch(arg->a_type)
      {
        case A_SYM:
          v = std::string(atom_getsym(arg)->s_name);
          break;
        case A_FLOAT:
          v = ossia::value(atom_getfloat(arg));
          break;
        case A_LONG:
          v = ossia::value(static_cast<long>(atom_getlong(arg)));
          break;
        default:;
      }

      ossia::value vv;
      parameter_base* xparam = (parameter_base*)owner;
      if(xparam->m_local_unit != std::nullopt)
      {
        auto src_unit = *xparam->m_local_unit;
        auto dst_unit = param->get_unit();

        vv = ossia::convert(v, src_unit, dst_unit);
      }
      else
        vv = v;

      x->push_parameter_value(param, std::move(vv));
    }
    else
    {
      std::vector<ossia::value> list;
      list.reserve(argc + 1);

      // shift to remove first argument which is the target address
      argc--;
      argv++;

      for(; argc > 0; argc--, argv++)
      {
        switch(argv->a_type)
        {
          case A_SYM:
            list.push_back(std::string(atom_getsym(argv)->s_name));
            break;
          case A_FLOAT:
            list.push_back(atom_getfloat(argv));
            break;
          case A_LONG:
            list.push_back(static_cast<long>(atom_getlong(argv)));
            break;
          default:
            object_error((t_object*)x, "value type not handled");
        }
      }
      parameter_base* xparam = (parameter_base*)owner;
      auto src_unit = *xparam->m_local_unit;
      auto dst_unit = param->get_unit();

      ossia::convert(list, src_unit, dst_unit);

      x->push_parameter_value(param, std::move(list));
    }
  }
}

void parameter_base::bang(parameter_base* x)
{
  for(auto m : x->m_node_selection)
  {
    auto param = m->get_node()->get_parameter();

    if(param->get_value_type() == ossia::val_type::IMPULSE)
    {
      x->push_parameter_value(param, ossia::impulse{});
    }
    else
    {
      m->output_value(param->value());
    }
  }
}

void parameter_base::in_float(parameter_base* x, double f)
{
  t_atom a;
  A_SETFLOAT(&a, f);
  parameter_base::push(x, nullptr, 1, &a);
}

void parameter_base::in_int(parameter_base* x, long int f)
{
  t_atom a;
  A_SETLONG(&a, f);
  parameter_base::push(x, nullptr, 1, &a);
}

void parameter_base::in_symbol(parameter_base* x, t_symbol* f)
{
  t_atom a;
  A_SETSYM(&a, f);
  parameter_base::push(x, nullptr, 1, &a);
}

void parameter_base::class_setup(t_class* c)
{
  object_base ::class_setup(c);

  class_addmethod(c, (method)parameter_base::push, "set", A_GIMME, 0);
  class_addmethod(c, (method)parameter_base::push, "anything", A_GIMME, 0);
  class_addmethod(c, (method)parameter_base::push_one, "send", A_GIMME, 0);
  class_addmethod(c, (method)parameter_base::bang, "bang", A_NOTHING, 0);

  class_addmethod(c, (method)parameter_base::in_int, "int", A_LONG, A_GIMME, 0);
  class_addmethod(c, (method)parameter_base::in_float, "float", A_FLOAT, 0);
  class_addmethod(c, (method)parameter_base::in_symbol, "symbol", A_SYM, 0);
  class_addmethod(c, (method)parameter_base::push, "list", A_GIMME, 0);
  class_addmethod(c, (method)parameter_base::push_default_value, "reset", A_NOTHING, 0);

  CLASS_ATTR_SYM(c, "unit", 0, parameter_base, m_unit);
  CLASS_ATTR_ENUM(
      c, "unit", 0,
      "gain.linear gain.midigain gain.db gain.db-raw time.second time.bark time.bpm "
      "time.cents time.hz time.mel time.midinote time.ms color.argb color.rgba "
      "color.rgb color.bgr color.argb8 color.rgba8 color.hsv color.cmy8 color.xyz "
      "position.cart3D position.cart2D position.spherical position.polar "
      "position.openGL position.cylindrical orientation.quaternion orientation.euler "
      "orientation.axis angle.degree angle.radian  time.speed distance.m distance.km "
      "distance.dm distance.cm distance.mm distance.um distance.nm distance.pm "
      "distance.inches distance.feet distance.miles speed.m/s speed.mph speed.km/h "
      "speed.kn speed.ft/s speed.ft/h");
  // maybe this enum could be done more properly by retrieving the full list from the
  // dataspace code ? sure, but I don't have any idea how to do that - PB
  CLASS_ATTR_LABEL(c, "unit", 0, "Value Unit");

  CLASS_ATTR_FLOAT(c, "rate", 0, parameter_base, m_rate);
  CLASS_ATTR_FILTER_MIN(c, "rate", 1);
  CLASS_ATTR_LABEL(c, "rate", 0, "Update Rate");

  CLASS_ATTR_LONG(c, "mute", 0, parameter_base, m_mute);
  CLASS_ATTR_STYLE(c, "mute", 0, "onoff");
  CLASS_ATTR_LABEL(c, "mute", 0, "Mute Output");

  CLASS_ATTR_SYM(c, "type", 0, parameter_base, m_type);
  CLASS_ATTR_ENUM(c, "type", 0, "float int bool symbol vec2f vec3f vec4f list impulse");
  CLASS_ATTR_LABEL(c, "type", 0, "Value Type");

  CLASS_ATTR_SYM(c, "clip", 0, parameter_base, m_bounding_mode);
  CLASS_ATTR_ENUM(c, "clip", 0, "free low high both wrap fold ");
  CLASS_ATTR_LABEL(c, "clip", 0, "Clip Mode");

  CLASS_ATTR_SYM(c, "mode", 0, parameter_base, m_access_mode);
  CLASS_ATTR_ENUM(c, "mode", 0, "bi get set");
  CLASS_ATTR_LABEL(c, "mode", 0, "Access Mode");

  CLASS_ATTR_ATOM_VARSIZE(
      c, "default", 0, parameter_base, m_default, m_default_size,
      OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "default", 0, "Default Value");

  CLASS_ATTR_ATOM_VARSIZE(
      c, "range", 0, parameter_base, m_range, m_range_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "range", 0, "Value Range");

  CLASS_ATTR_ATOM_VARSIZE(
      c, "min", 0, parameter_base, m_min, m_min_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "min", 0, "Minimum Value");

  CLASS_ATTR_ATOM_VARSIZE(
      c, "max", 0, parameter_base, m_max, m_max_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "max", 0, "Maximum Value");

  CLASS_ATTR_LONG(c, "repetitions", 0, parameter_base, m_repetitions);
  CLASS_ATTR_STYLE(c, "repetitions", 0, "onoff");
  CLASS_ATTR_LABEL(c, "repetitions", 0, "Allow Repetitions");

  CLASS_ATTR_LONG(c, "enable", 0, parameter_base, m_enable);
  CLASS_ATTR_STYLE(c, "enable", 0, "onoff");
  CLASS_ATTR_LABEL(c, "enable", 0, "Enable Parameter");

  CLASS_ATTR_LONG(c, "invisible", 0, parameter_base, m_invisible);
  CLASS_ATTR_STYLE(c, "invisible", 0, "onoff");
  CLASS_ATTR_LABEL(c, "invisible", 0, "Hide Parameter");

  CLASS_ATTR_LONG(c, "critical", 0, parameter_base, m_critical);
  CLASS_ATTR_STYLE(c, "critical", 0, "onoff");
  CLASS_ATTR_LABEL(c, "critical", 0, "Critical");
}

parameter_base::parameter_base()
{
  m_type = gensym("float");
  m_bounding_mode = gensym("free");
  m_access_mode = gensym("bi");
  m_description = gensym("");
  m_unit = gensym("");
}

void parameter_base::output_values(bool only_default)
{
  // TODO unify this with fire_all_values_recursively() (cf utils.cpp)
  std::vector<node_priority> priority_graph;
  priority_graph.reserve(m_matchers.size());

  for(const auto& m : m_matchers)
  {
    auto node = m->get_node();
    if(node)
    {
      auto prio = get_priority_list(node);
      priority_graph.push_back({m.get(), prio});
    }
  }

  fire_values_by_priority(priority_graph, only_default);
}

} // namespace max
} // namespace ossia
