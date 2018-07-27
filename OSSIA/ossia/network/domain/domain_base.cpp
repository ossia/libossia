// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "domain_base.hpp"
#include <ossia/network/domain/detail/apply_domain.hpp>
#include <ossia/network/domain/detail/min_max.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/detail/small_vector.hpp>

namespace ossia
{
value get_min(const domain& dom)
{
  return ossia::apply(domain_min_visitor{}, dom.v);
}

value get_max(const domain& dom)
{
  return ossia::apply(domain_max_visitor{}, dom.v);
}

void set_min(domain& dom, const ossia::value& val)
{
  if (dom && val.valid())
    return ossia::apply(domain_set_min_visitor{}, dom.v, val.v);
  else if (dom && !val.valid()) // Remove the value
    return ossia::apply_nonnull(domain_set_min_visitor{}, dom.v);
  else if(!dom && val.valid())
    dom = ossia::apply_nonnull(domain_min_creation_visitor{}, val.v);
}

void set_max(domain& dom, const ossia::value& val)
{
  if (dom && val.valid())
    return ossia::apply(domain_set_max_visitor{}, dom.v, val.v);
  else if (dom)
    return ossia::apply_nonnull(domain_set_max_visitor{}, dom.v);
  else if(!dom && val.valid())
    dom = ossia::apply_nonnull(domain_max_creation_visitor{}, val.v);
}

void set_values(domain& dom, const std::vector<ossia::value>& val)
{
  if (dom)
    return ossia::apply_nonnull(value_set_update_visitor{val}, dom.v);
}

std::vector<ossia::value> get_values(const domain& dom)
{
  if (dom)
    return ossia::apply_nonnull(value_set_get_visitor{}, dom.v);
  return {};
}


domain make_domain(const ossia::value& min, const ossia::value& max)
{
  if (min.valid() && max.valid())
  {
    return ossia::apply(domain_minmax_creation_visitor{}, min.v, max.v);
  }
  else if (min.valid())
  {
    auto dom = ossia::apply(domain_minmax_creation_visitor{}, min.v, min.v);
    set_max(dom, ossia::value{});
    return dom;
  }
  else if (max.valid())
  {
    auto dom = ossia::apply(domain_minmax_creation_visitor{}, max.v, max.v);
    set_min(dom, ossia::value{});
    return dom;
  }
  return {};
}

domain make_domain(
    const std::vector<std::string>& s)
{
  domain_base<std::string> v;
  v.values.insert(s.begin(), s.end());
  return domain{std::move(v)};
}

domain make_domain(const std::vector<ossia::value>& vals)
{
  if (vals.size() > 0)
  {
    auto dom = init_domain(vals[0].getType());
    ossia::apply_nonnull(value_set_update_visitor{vals}, dom.v);
    return dom;
  }

  return {};
}
domain make_domain(
    const ossia::value& min, const ossia::value& max,
    const std::vector<ossia::value>& vals)
{
  if (min.valid() && max.valid())
  {
    auto dom = ossia::apply(domain_minmax_creation_visitor{}, min.v, max.v);
    set_values(dom, vals);
    return dom;
  }
  else if (min.valid())
  {
    auto dom = ossia::apply(domain_minmax_creation_visitor{}, min.v, min.v);
    set_max(dom, ossia::value{});
    set_values(dom, vals);
    return dom;
  }
  else if (max.valid())
  {
    auto dom = ossia::apply(domain_minmax_creation_visitor{}, max.v, max.v);
    set_min(dom, ossia::value{});
    set_values(dom, vals);
    return dom;
  }
  else
  {
    if (vals.size() > 0)
    {
      auto dom = init_domain(vals[0].getType());
      ossia::apply_nonnull(value_set_update_visitor{vals}, dom.v);
      return dom;
    }
  }
  return {};
}

domain make_domain_from_osc(
    const ossia::small_vector<ossia::value, 2>& val, const ossia::value& cur)
{
  if (val.size() == 2 && val[0].valid() && val[1].valid())
  {
    return ossia::apply(domain_minmax_creation_visitor{}, val[0].v, val[1].v);
  }
  else
  {
    return cur.apply(domain_value_set_creation_visitor{val});
  }
}

ossia::domain make_domain_from_minmax(const std::vector<ossia::value>& min, const std::vector<ossia::value>& max, ossia::val_type v)
{
  const auto s1 = min.size();
  const auto s2 = max.size();

  if(s1 == s2)
  {
    switch(s1)
    {
      case 0:
        return {};
      case 1:
        return ossia::make_domain(min[0], max[0]);
      case 2:
        return ossia::make_domain(convert<ossia::vec2f>(min), convert<ossia::vec2f>(max));
      case 3:
        return ossia::make_domain(convert<ossia::vec3f>(min), convert<ossia::vec3f>(max));
      case 4:
        return ossia::make_domain(convert<ossia::vec4f>(min), convert<ossia::vec4f>(max));
      default:
        return ossia::make_domain(ossia::value(min), ossia::value(max));
    }
  }
  else if(s1 == 0)
  {
    switch( v )
    {
      case ossia::val_type::CHAR:
        return ossia::make_domain((char)0, max[0]);
      case ossia::val_type::FLOAT:
        return ossia::make_domain((float)0, max[0]);
      case ossia::val_type::INT:
        return ossia::make_domain((int)0, max[0]);
      case ossia::val_type::VEC2F:
        return ossia::make_domain(ossia::make_vec(0, 0), convert<ossia::vec2f>(max));
      case ossia::val_type::VEC3F:
        return ossia::make_domain(ossia::make_vec(0, 0, 0), convert<ossia::vec3f>(max));
      case ossia::val_type::VEC4F:
        return ossia::make_domain(ossia::make_vec(0, 0, 0, 0), convert<ossia::vec4f>(max));
      default:
        break;
    }
  }
  else if(s2 == 0)
  {
    switch( v )
    {
      case ossia::val_type::CHAR:
        return ossia::make_domain(min[0], (char) 255);
      case ossia::val_type::FLOAT:
        return ossia::make_domain(min[0], (float) 1);
      case ossia::val_type::INT:
        return ossia::make_domain(min[0], (int) 1);
      case ossia::val_type::VEC2F:
        return ossia::make_domain(convert<ossia::vec2f>(min), ossia::make_vec(1, 1));
      case ossia::val_type::VEC3F:
        return ossia::make_domain(convert<ossia::vec3f>(min), ossia::make_vec(1, 1, 1));
      case ossia::val_type::VEC4F:
        return ossia::make_domain(convert<ossia::vec4f>(min), ossia::make_vec(1, 1, 1, 1));
      default:
        break;
    }
  }
    // weird case where min and max don't have the same number of values but why not
  return ossia::make_domain(ossia::value(min),ossia::value(max));
}

value apply_domain(const domain& dom, bounding_mode b, const ossia::value& val)
{
  if (bool(dom) && bool(val.v))
  {
    return ossia::apply(apply_domain_visitor{b}, val.v, dom.v);
  }
  return val;
}

value apply_domain(const domain& dom, bounding_mode b, ossia::value&& val)
{
  if (bool(dom) && bool(val.v) && b != ossia::bounding_mode::FREE)
  {
    return ossia::apply(apply_domain_visitor{b}, ossia::move(val.v), dom.v);
  }
  return val;
}

domain init_domain(ossia::val_type type)
{
  switch (type)
  {
    case val_type::IMPULSE:
      return domain_base<impulse>{};
    case val_type::INT:
      return domain_base<int32_t>{};
    case val_type::FLOAT:
      return domain_base<float>{};
    case val_type::BOOL:
      return domain_base<bool>{};
    case val_type::CHAR:
      return domain_base<char>{};
    case val_type::STRING:
      return domain_base<std::string>();
    case val_type::LIST:
      return vector_domain();
    case val_type::VEC2F:
      return vecf_domain<2>();
    case val_type::VEC3F:
      return vecf_domain<3>();
    case val_type::VEC4F:
      return vecf_domain<4>();
    default:
      return domain{};
  }
}
}
