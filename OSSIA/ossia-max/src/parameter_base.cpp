// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia-max/src/parameter_base.hpp>

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_attributes.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/common/complex_type.hpp>
#include <ossia-max/src/utils.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <algorithm>

namespace ossia {
namespace max {

void parameter_base::update_attribute(parameter_base* x, ossia::string_view attribute, const ossia::net::node_base* node)
{
  auto matchers = make_matchers_vector(x,node);

  if (matchers.empty())
    return;

  if ( attribute == ossia::net::text_refresh_rate() ){
    get_rate(x, matchers);
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
  }  else if ( attribute == ossia::net::text_extended_type() ){
    get_type(x, matchers);
    get_unit(x, matchers);
  } else if ( attribute == ossia::net::text_muted() ){
    get_mute(x, matchers);
  } else if ( attribute == ossia::net::text_unit() ){
    get_unit(x, matchers);
  } else {
    object_base::update_attribute((node_base*)x, attribute, node);
  }
}

t_max_err parameter_base::notify(parameter_base *x, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
  t_symbol *attrname;

  if (!x->m_lock && msg == gensym("attr_modified")) {
    attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));

    if ( attrname == gensym("mode") )
      x->set_access_mode();
    else if ( attrname == gensym("repetitions") )
      x->set_repetition_filter();
    else if ( attrname == gensym("enable") )
      x->set_enable();
    else if ( attrname == gensym("type") )
      x->set_type();
    else if( attrname == gensym("rate") )
      x->set_rate();
    else if( attrname == gensym("range") )
      x->set_range();
    else if ( attrname == gensym("clip") )
      x->set_bounding_mode();
    else if ( attrname == gensym("min") || attrname == gensym("max") )
      x->set_minmax();
    else if ( attrname == gensym("default") )
      x->set_default();
    else
      object_base::notify(x, s, msg, sender, data);
  }
  return 0;
}

void parameter_base::set_access_mode()
{
  for (auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    auto param = node->get_parameter();

    std::string access_mode = m_access_mode->s_name;
    boost::algorithm::to_lower(access_mode);
    m_access_mode = gensym(access_mode.c_str());

    param->set_access(symbol2access_mode(m_access_mode));
  }
}

void parameter_base::set_repetition_filter()
{
  for (auto m : m_node_selection)
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
  for (auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_disabled(*node, !m_enable);
  }
}

void parameter_base::set_type()
{
  for (auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::try_setup_parameter(m_type->s_name, *node);
  }
}

void parameter_base::set_rate()
{
  for (auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::set_refresh_rate(*node,m_rate);
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

  for (t_matcher* m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    auto min = _min;
    auto max = _max;

    switch(param->get_value_type())
    {
      case ossia::val_type::BOOL:
      case ossia::val_type::CHAR:
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
      default:
        continue;
    }

    param->set_domain(make_domain_from_minmax(min, max, param->get_value_type()));
  }
}

void parameter_base::set_unit()
{
  for (auto m : m_node_selection)
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
        object_error((t_object*)this, "wrong unit: %s", m_unit->s_name);
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

void parameter_base::push_default_value(parameter_base* x)
{
  ossia::net::node_base* node;

  if (!x->m_mute)
  {
    for (auto& m : x->m_node_selection)
    {
      node = m->get_node();
      auto param = node->get_parameter();

      auto def_val = ossia::net::get_default_value(*node);
      if (def_val)
      {
        param->push_value(*def_val);
        trig_output_value(node);
      }
    }
  }
}

void parameter_base::set_range()
{
  if(m_range_size == 0)
    return;

  for (auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    if ( param->get_value_type() == ossia::val_type::STRING )
    {
      std::vector<std::string> senum;
      for ( int i = 0; i < m_range_size; i++)
      {
        if (m_range[i].a_type == A_SYM)
          senum.push_back(m_range[i].a_w.w_sym->s_name);
        else if (m_range[i].a_type == A_FLOAT)
        {
          senum.push_back(fmt::format("{}", m_range[i].a_w.w_float));
        }
        else if (m_range[i].a_type == A_LONG)
        {
          senum.push_back(fmt::format("{}", m_range[i].a_w.w_long));
        }
        else
          break;
      }
      param->set_domain(make_domain(senum));
    }
    else if (   ( m_range[0].a_type == A_FLOAT || m_range[0].a_type == A_LONG )
                && ( m_range[1].a_type == A_FLOAT || m_range[1].a_type == A_LONG ) )
    {
      float fmin = atom_getfloat(m_range);
      float fmax = atom_getfloat(m_range+1);
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
          // FIXME use something like MAX_ATTR_SIZE instead of arbitrary constant
          min.resize(OSSIA_MAX_MAX_ATTR_SIZE);
          max.resize(OSSIA_MAX_MAX_ATTR_SIZE);
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
  for (auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    std::string bounding_mode = m_bounding_mode->s_name;
    boost::algorithm::to_lower(bounding_mode);
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
      object_error((t_object*)this, "unknown clip mode: %s", bounding_mode.c_str());
    }
  }
}

template<std::size_t N>
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
  for (auto m : m_node_selection)
  {
    ossia::net::node_base* node = m->get_node();
    ossia::net::parameter_base* param = node->get_parameter();

    switch(param->get_value_type())
    {
      case ossia::val_type::VEC4F:
        if (m_default_size >= 4)
          ossia::net::set_default_value(*node, max::to_vec<4>(m_default));
        break;
      case ossia::val_type::VEC3F:
        if (m_default_size >= 3)
          ossia::net::set_default_value(*node, to_vec<3>(m_default));
        break;
      case ossia::val_type::VEC2F:
        if (m_default_size >= 2)
          ossia::net::set_default_value(*node, to_vec<2>(m_default));
        break;
      case ossia::val_type::FLOAT:
      case ossia::val_type::CHAR:
      case ossia::val_type::INT:
      case ossia::val_type::BOOL:
      {
        if (m_default[0].a_type == A_FLOAT )
        {
          ossia::net::set_default_value(*node, m_default[0].a_w.w_float);
        }
        else if (m_default[0].a_type == A_LONG )
        {
          ossia::net::set_default_value(*node, (int)m_default[0].a_w.w_long);
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
        if (m_default_size > 0)
        {
          auto def = attribute2value(m_default, m_default_size);
          ossia::net::set_default_value(*node, def);
        }
        break;
      }
      default:
        ;
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
  else if ( s == gensym("queue_length") )
    parameter_base::get_queue_length(x,x->m_node_selection);
  else
    object_base::get_mess_cb(x,s);


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
  lock_and_touch(x, gensym("range"));
  lock_and_touch(x, gensym("min"));
  lock_and_touch(x, gensym("max"));
}

void parameter_base::get_bounding_mode(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_bounding_mode = bounding_mode2symbol(param->get_bounding());
    t_atom a;
    A_SETSYM(&a,x->m_bounding_mode);
    outlet_anything(x->m_dumpout, gensym("clip"), 1, &a);
  }
  lock_and_touch(x, gensym("clip"));
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

      x->m_default_size = va.size() > 512 ? 512 : va.size();

      for (int i=0; i < x->m_default_size; i++ )
        x->m_default[i] = va[i];
    } else {
      x->m_default_size = 0;
    }

    outlet_anything(x->m_dumpout, gensym("default"),
                    x->m_default_size, x->m_default);
  }
  lock_and_touch(x, gensym("default"));

}

void parameter_base::get_type(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_type = val_type2symbol(param->get_value_type());

    t_atom a;
    A_SETSYM(&a,x->m_type);
    outlet_anything(x->m_dumpout, gensym("type"), 1, &a);
  }
  lock_and_touch(x, gensym("type"));
}

void parameter_base::get_access_mode(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_access_mode = access_mode2symbol(param->get_access());

    t_atom a;
    A_SETSYM(&a, x->m_access_mode);
    outlet_anything(x->m_dumpout, gensym("mode"), 1, &a);
  }
  lock_and_touch(x, gensym("mode"));
}

void parameter_base::get_repetition_filter(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_repetitions = !param->get_repetition_filter();

    t_atom a;
    A_SETLONG(&a, x->m_repetitions);
    outlet_anything(x->m_dumpout, gensym("repetitions"), 1, &a);
  }
  lock_and_touch(x, gensym("repetitions"));
}

void parameter_base::get_enable(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    auto param = m->get_node()->get_parameter();
    x->m_enable = !param->get_disabled();

    t_atom a;
    A_SETLONG(&a,x->m_enable);
    outlet_anything(x->m_dumpout, gensym("enable"), 1, &a);
  }
  lock_and_touch(x, gensym("enable"));
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
    A_SETSYM(&a, x->m_unit);
    outlet_anything(x->m_dumpout, gensym("unit"), 1, &a);
  }
  lock_and_touch(x, gensym("unit"));
}

void parameter_base::get_mute(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    ossia::net::parameter_base* param = m->get_node()->get_parameter();

    x->m_mute = param->get_muted();

    t_atom a;
    A_SETLONG(&a, x->m_mute);
    outlet_anything(x->m_dumpout, gensym("mute"), 1, &a);
  }
  lock_and_touch(x, gensym("mute"));
}

void parameter_base::get_queue_length(parameter_base*x, std::vector<t_matcher*> nodes)
{
  for (auto m : nodes)
  {
    outlet_anything(x->m_dumpout, gensym("address"), 1, m->get_atom_addr_ptr());

    t_atom a;
    A_SETLONG(&a, x->m_queue_length);
    outlet_anything(x->m_dumpout, gensym("queue_length"), 1, &a);
  }
  lock_and_touch(x, gensym("queue_length"));
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
      A_SETFLOAT(&a, x->m_rate);
      outlet_anything(x->m_dumpout, gensym("rate"), 1, &a);
    }
  }
  lock_and_touch(x, gensym("rate"));
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
      case A_LONG:
        arr[i] = (float)atom_getlong(&argv[i]);
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
    auto xparam = (parameter_base*)m->get_parent();

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
      param->push_value(v);
      if (set_flag)
        m->m_set_pool.push_back(param->value());
    }
    trig_output_value(node);
  }
}

void just_push(parameter_base* x, ossia::value&& v, bool set_flag = false)
{
  for (auto& m : x->m_matchers)
  {
    auto node = m->get_node();
    auto param = node->get_parameter();
    if (set_flag) m->m_set_pool.push_back(v);
    param->push_value(v);
    trig_output_value(node);
  }
}

void parameter_base::push(parameter_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if (x->m_mute)
    return;

  bool set_flag = false;

  if (s && s == gensym("set"))
    set_flag = true;

  if (argc == 0 && s)
  {
    just_push(x, std::string(s->s_name), set_flag);
  }
  else if (argc == 1 && s &&
           ( s == gensym("float") || s == gensym("list") || ( s == gensym("int"))))
  {
    // convert one element array to single element
    switch(argv->a_type)
    {
      case A_SYM:
        just_push(x, std::string(atom_getsym(argv)->s_name), set_flag);
        break;
      case A_FLOAT:
        convert_or_push(x, ossia::value(atom_getfloat(argv)), set_flag);
        break;
      case A_LONG:
        convert_or_push(x, static_cast<int32_t>(atom_getlong(argv)), set_flag);
        break;
      default:
        return;
    }
  }
  else
  {

    std::vector<ossia::value> list;
    list.reserve(argc+1);

    bool start_with_symbol = false;

    if ( s && s != gensym("list") && s != gensym("set")  )
    {
      list.push_back(std::string(s->s_name));
      start_with_symbol = true;
    }

    bool is_array = false;

    /*
    if(!start_with_symbol)
    {
      switch(argc)
      {
        case 2:
          if(auto arr = to_array<2>(argv)) {
            is_array = true;
            convert_or_push(x, *arr, set_flag);
          }
          break;
        case 3:
          if(auto arr = to_array<3>(argv)) {
            is_array = true;
            convert_or_push(x, *arr, set_flag);
          }
          break;
        case 4:
          if(auto arr = to_array<4>(argv)) {
            is_array = true;
            convert_or_push(x, *arr, set_flag);
          }
          break;
      }
    }
    */

    if (!is_array)
    {
      for (; argc > 0; argc--, argv++)
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
      convert_or_push(x, std::move(list), set_flag);
    }
  }

  // go through all matchers to fire the new value
  for (auto node : x->m_node_selection)
  {
    // there should be only one param with that node
    // so break asap
    if (x->m_otype == object_class::param )
    {
      node->output_value();
    }
    else
    {
      for(auto param : ossia_max::instance().parameters.reference())
      {
        bool break_flag = false;

        for (auto& m : param->m_matchers)
        {
          if ( *m == *node )
          {
            m->output_value();
            break_flag = true;
            break;
          }
          if (break_flag)
            break;
        }
      }
    }

    for(auto remote : ossia_max::instance().remotes.reference())
    {
      for (auto& m : remote->m_matchers)
      {
        if ( *m == *node )
          m->output_value();
      }
    }
  }
}


void parameter_base::push_one(parameter_base* x, t_symbol* s, int argc, t_atom* argv)
{
  if(!s || argc < 2 || argv[0].a_type != A_SYM)
    return;

  auto target = atom_getsym(&argv[0])->s_name;
  object_base* parent{};
  ossia::net::node_base* node{};
  if (!x->m_mute)
  {
    for (const auto m : x->m_node_selection)
    {
      auto cur = m->get_node();
      auto addr = ossia::net::address_string_from_node(*cur);
      if(boost::algorithm::ends_with(addr, target)) {
        node = cur;
        parent = m->get_parent();
        break;
      }
    }

    if(!node || !parent)
      return;

    auto param = node->get_parameter();
    if(!param)
      return;

    if (argc == 2)
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
        default:
          ;
      }

      ossia::value vv;
      parameter_base* xparam = (parameter_base*)parent;
      if ( xparam->m_ounit != ossia::none )
      {
        auto src_unit = *xparam->m_ounit;
        auto dst_unit = param->get_unit();

        vv = ossia::convert(v, src_unit, dst_unit);
      } else
        vv = v;

      param->push_value(std::move(vv));
    }
    else
    {
      std::vector<ossia::value> list;
      list.reserve(argc+1);

      for (; argc > 1; argc--, argv++)
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
      parameter_base* xparam = (parameter_base*) parent;
      auto src_unit = *xparam->m_ounit;
      auto dst_unit = param->get_unit();

      ossia::convert(list, src_unit, dst_unit);

      param->push_value(std::move(list));
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
    m->output_value();
  }
  clock_delay(x->m_poll_clock, x->m_rate);
}

void parameter_base::in_float(parameter_base* x, double f)
{
  t_atom a;
  A_SETFLOAT(&a,f);
  parameter_base::push(x,nullptr,1,&a);
}

void parameter_base::in_int(parameter_base* x, long int f)
{
  t_atom a;
  A_SETLONG(&a,f);
  parameter_base::push(x,nullptr,1,&a);
}

void parameter_base::in_symbol(parameter_base* x, t_symbol* f)
{
  t_atom a;
  A_SETSYM(&a,f);
  parameter_base::push(x,nullptr,1,&a);
}

void parameter_base::class_setup(t_class* c)
{
  object_base :: class_setup(c);

  class_addmethod(c, (method) parameter_base::push,  "set",     A_GIMME, 0);
  class_addmethod(c, (method) parameter_base::push, "anything", A_GIMME, 0);
  class_addmethod(c, (method) parameter_base::push_one, "send", A_GIMME, 0);
  class_addmethod(c, (method) parameter_base::bang, "bang",     A_NOTHING,  0);

  class_addmethod(
        c, (method)parameter_base::in_int,
        "int", A_LONG, A_GIMME, 0);
  class_addmethod(
        c, (method)parameter_base::in_float,
        "float", A_FLOAT, 0);
  class_addmethod(
        c, (method)parameter_base::in_symbol,
        "symbol", A_SYM, 0);
  class_addmethod(
        c, (method)parameter_base::push,
        "list", A_GIMME, 0);
  class_addmethod(
        c, (method)parameter_base::push_default_value,
        "reset", A_NOTHING, 0);

  CLASS_ATTR_SYM(
        c, "unit", 0, parameter_base, m_unit);
  CLASS_ATTR_ENUM (
        c, "unit", 0, "gain.linear gain.midigain gain.db gain.db-raw time.second time.bark time.bpm time.cents time.hz time.mel time.midinote time.ms color.argb color.rgba color.rgb color.bgr color.argb8 color.rgba8 color.hsv color.cmy8 color.xyz position.cart3D position.cart2D position.spherical position.polar position.openGL position.cylindrical orientation.quaternion orientation.euler orientation.axis angle.degree angle.radian  time.speed distance.m distance.km distance.dm distance.cm distance.mm distance.um distance.nm distance.pm distance.inches distance.feet distance.miles speed.m/s speed.mph speed.km/h speed.kn speed.ft/s speed.ft/h");
  //maybe this enum could be done more properly by retrieving the full list from the dataspace code ?
  //sure, but I don't have any idea how to do that - PB
  CLASS_ATTR_LABEL(c, "unit", 0, "Value Unit");

  CLASS_ATTR_FLOAT(
        c, "rate", 0, parameter_base, m_rate);
  CLASS_ATTR_FILTER_MIN(c, "rate", 1);
  CLASS_ATTR_LABEL(c, "rate", 0, "Update Rate");

  CLASS_ATTR_LONG(
        c, "mute", 0, parameter_base, m_mute);
  CLASS_ATTR_STYLE(
        c, "mute", 0, "onoff");
  CLASS_ATTR_LABEL(c, "mute", 0, "Mute Output");

  CLASS_ATTR_LONG(
        c, "queue_length", 0, parameter_base, m_queue_length);
  CLASS_ATTR_LABEL(c, "queue_length", 0, "Message queue length");

  CLASS_ATTR_SYM(
        c, "type", 0, parameter_base, m_type);
  CLASS_ATTR_ENUM (
        c, "type", 0, "float int bool symbol vec2f vec3f vec4f list impulse");
  CLASS_ATTR_LABEL(c, "type", 0, "Value Type");

  CLASS_ATTR_SYM(
        c, "clip", 0, parameter_base,
        m_bounding_mode);
  CLASS_ATTR_ENUM (
        c, "clip", 0, "free low high both wrap fold ");
  CLASS_ATTR_LABEL(c, "clip", 0, "Clip Mode");

  CLASS_ATTR_SYM(
        c, "mode", 0, parameter_base,
        m_access_mode);
  CLASS_ATTR_ENUM (
        c, "mode", 0, "bi get set");
  CLASS_ATTR_LABEL(c, "mode", 0, "Acces Mode");

  CLASS_ATTR_ATOM_VARSIZE(
        c, "default", 0, parameter_base,
        m_default, m_default_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "default", 0, "Default Value");

  CLASS_ATTR_ATOM_VARSIZE(
        c, "range", 0, parameter_base,
        m_range, m_range_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "range", 0, "Value Range");

  CLASS_ATTR_ATOM_VARSIZE(
        c, "min", 0, parameter_base,
        m_min, m_min_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "min", 0, "Minimum Value");

  CLASS_ATTR_ATOM_VARSIZE(
        c, "max", 0, parameter_base,
        m_max, m_max_size, OSSIA_MAX_MAX_ATTR_SIZE);
  CLASS_ATTR_LABEL(c, "max", 0, "Maximum Value");

  CLASS_ATTR_LONG(
        c, "repetitions", 0, parameter_base,
        m_repetitions);
  CLASS_ATTR_STYLE(
        c, "repetitions", 0, "onoff");
  CLASS_ATTR_LABEL(c, "repetitions", 0, "Allow Repetitions");

  CLASS_ATTR_LONG(c, "enable", 0, parameter_base, m_enable);
  CLASS_ATTR_STYLE(c, "enable", 0, "onoff");
  CLASS_ATTR_LABEL(c, "enable", 0, "Enable Parameter");

  CLASS_ATTR_LONG(c, "invisible", 0, parameter_base, m_invisible);
  CLASS_ATTR_STYLE(
        c, "invisible", 0, "onoff");
  CLASS_ATTR_LABEL(c, "invisible", 0, "Hide Parameter");
}

parameter_base::parameter_base()
{
  m_type = gensym("float");
  m_bounding_mode = gensym("free");
  m_access_mode = gensym("bi");
  m_description = gensym("");
  m_unit = gensym("");

  m_range_size = 2;
  atom_setfloat(m_range,0.);
  atom_setfloat(m_range+1,1.);
}

} // namespace max
} // namespace ossia
