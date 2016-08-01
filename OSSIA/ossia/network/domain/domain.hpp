#pragma once
#include <ossia/network/domain/detail/domain_visitors.hpp>
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
namespace net
{
inline value clamp(const domain& dom, bounding_mode b, const value& val)
{
  if (bool(dom) && bool(val.v))
  {
    return eggs::variants::apply(domain_clamp_visitor{b}, val.v, dom);
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

inline domain makeDomain(const ossia::value& min, const ossia::value& max)
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
}
}
