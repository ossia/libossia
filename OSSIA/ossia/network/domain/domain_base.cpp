#include "domain_base.hpp"
#include <boost/container/small_vector.hpp>
#include <ossia/network/domain/detail/apply_domain.hpp>
#include <ossia/network/domain/detail/min_max.hpp>
#include <ossia/network/domain/domain.hpp>

namespace ossia
{
namespace net
{
value get_min(const domain& dom)
{
  return ossia::apply(domain_min_visitor{}, dom);
}

value get_max(const domain& dom)
{
  return ossia::apply(domain_max_visitor{}, dom);
}

void set_min(domain& dom, const ossia::value& val)
{
  if(dom && val.valid())
    return eggs::variants::apply(domain_set_min_visitor{}, dom, val.v);
  else if(dom) // Remove the value
    return eggs::variants::apply(domain_set_min_visitor{}, dom);
}

void set_max(domain& dom, const ossia::value& val)
{
  if(dom && val.valid())
    return eggs::variants::apply(domain_set_max_visitor{}, dom, val.v);
  else if(dom)
    return eggs::variants::apply(domain_set_max_visitor{}, dom);
}

domain make_domain(const ossia::value& min, const ossia::value& max)
{
  if (min.valid() && max.valid())
  {
    return eggs::variants::apply(domain_minmax_creation_visitor{}, min.v, max.v);
  }
  return {};
}

domain make_domain(
    const boost::container::small_vector<ossia::value, 2>& val,
    const ossia::value& cur)
{
  if (val.size() == 2 && val[0].valid() && val[1].valid())
  {
    return eggs::variants::apply(domain_minmax_creation_visitor{}, val[0].v, val[1].v);
  }
  else
  {
    return eggs::variants::apply(domain_value_set_creation_visitor{val}, cur.v);
  }
}

bool
operator==(const domain_base<Impulse>& lhs, const domain_base<Impulse>& rhs)
{
  return true;
}
bool
operator==(const domain_base<int32_t>& lhs, const domain_base<int32_t>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<float>& lhs, const domain_base<float>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<bool>& lhs, const domain_base<bool>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<char>& lhs, const domain_base<char>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<std::vector<ossia::value>>& lhs, const domain_base<std::vector<ossia::value>>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Vec2f>& lhs, const domain_base<Vec2f>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Vec3f>& lhs, const domain_base<Vec3f>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Vec4f>& lhs, const domain_base<Vec4f>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<ossia::value>& lhs, const domain_base<ossia::value>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<std::string>& lhs, const domain_base<std::string>& rhs)
{
  return lhs.values == rhs.values;
}

value apply_domain(const domain& dom, bounding_mode b, const ossia::value& val)
{
  if (bool(dom) && bool(val.v))
  {
    return eggs::variants::apply(apply_domain_visitor{b}, val.v, dom);
  }
  return val;
}

value apply_domain(const domain& dom, bounding_mode b, ossia::value&& val)
{
  if (bool(dom) && bool(val.v))
  {
    return eggs::variants::apply(apply_domain_visitor{b}, ossia::move(val.v), dom);
  }
  return val;
}

domain init_domain(ossia::val_type type)
{
  switch (type)
  {
    case val_type::IMPULSE:
      return domain_base<Impulse>{};
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
    case val_type::TUPLE:
      return domain_base<std::vector<ossia::value>>();
    case val_type::VEC2F:
      return domain_base<Vec2f>();
    case val_type::VEC3F:
      return domain_base<Vec3f>();
    case val_type::VEC4F:
      return domain_base<Vec4f>();
    case val_type::DESTINATION:
    default:
      return domain{};
  }
}
}
}
