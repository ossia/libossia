// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "domain_base.hpp"

#include <ossia/detail/math.hpp>
#include <ossia/network/domain/detail/clamp_visitors.hpp>
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
struct clamp_max_functor
{
  template <typename... T>
  static OSSIA_INLINE auto compute(T&&... args)
  {
    return ossia::clamp_max(std::forward<T>(args)...);
  }
};

ossia::value clamp_max(const ossia::value& val, const ossia::value& max)
{
  if (val.valid() && max.valid())
    return ossia::apply(
        apply_binary_fun_visitor<clamp_max_functor>{}, val.v, max.v);
  return val;
}

#if defined(FAST_COMPILES)
ossia::value clamp_max(ossia::value&& val, const ossia::value& max)
{
  return clamp_max(val, max);
}
#else
ossia::value clamp_max(ossia::value&& val, const ossia::value& max)
{
  if (val.valid() && max.valid())
    return ossia::apply(
        apply_binary_fun_visitor<clamp_max_functor>{}, ossia::move(val).v,
        max.v);
  return val;
}
#endif
}
