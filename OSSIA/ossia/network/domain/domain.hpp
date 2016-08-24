#pragma once
#include <ossia/network/domain/detail/domain_visitors.hpp>
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
namespace net
{

template<typename Value_T>
value clamp(const domain& dom, bounding_mode b, Value_T&& val)
{
  if (bool(dom) && bool(val.v))
  {
    return eggs::variants::apply(domain_clamp_visitor{b}, std::forward<Value_T>(val).v, dom);
  }
  return val;
}

inline value min(const domain& dom)
{
  return ossia::apply(domain_min_visitor{}, dom);
}

inline value max(const domain& dom)
{
  return ossia::apply(domain_max_visitor{}, dom);
}

inline void set_min(domain& dom, const ossia::value& val)
{
  return ossia::apply(domain_set_min_visitor{val}, dom);
}

inline void set_max(domain& dom, const ossia::value& val)
{
  return ossia::apply(domain_set_max_visitor{val}, dom);
}

inline domain make_domain(const ossia::value& min, const ossia::value& max)
{
  if (min.valid() && max.valid())
  {
    return eggs::variants::apply(domain_creation_visitor{}, min.v, max.v);
  }
  return {};
}

inline bool
operator==(const domain_base<Int>& lhs, const domain_base<Int>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
inline bool
operator==(const domain_base<Float>& lhs, const domain_base<Float>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
inline bool
operator==(const domain_base<Bool>& lhs, const domain_base<Bool>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
inline bool
operator==(const domain_base<Char>& lhs, const domain_base<Char>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
inline bool
operator==(const domain_base<String>& lhs, const domain_base<String>& rhs)
{
  return lhs.values == rhs.values;
}

template <typename T>
bool operator==(const domain_base<T>& lhs, const domain_base<T>& rhs)
{
  return true;
}

inline domain init_domain(ossia::val_type type)
{
  switch (type)
  {
    case val_type::IMPULSE:
      return domain_base<Impulse>{};
    case val_type::INT:
      return domain_base<Int>{};
    case val_type::FLOAT:
      return domain_base<Float>{};
    case val_type::BOOL:
      return domain_base<Bool>{};
    case val_type::CHAR:
      return domain_base<Char>{};
    case val_type::STRING:
      return domain_base<String>();
    case val_type::TUPLE:
      return domain_base<Tuple>{};
    case val_type::VEC2F:
      return domain_base<Vec2f>{};
    case val_type::VEC3F:
      return domain_base<Vec3f>{};
    case val_type::VEC4F:
      return domain_base<Vec4f>{};
    case val_type::DESTINATION:
      return domain_base<Destination>{};
    case val_type::BEHAVIOR:
      return domain_base<Behavior>{};
    default:
      return domain{};
  }
}
}
}
