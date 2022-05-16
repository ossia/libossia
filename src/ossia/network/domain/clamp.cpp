// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "domain_base.hpp"

#include <ossia/detail/math.hpp>
#include <ossia/network/domain/detail/clamp_visitors.hpp>
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{

struct clamp_functor
{
  template <typename... T>
  static OSSIA_INLINE auto compute(T&&... args)
  {
    return ossia::clamp(std::forward<T>(args)...);
  }
};

ossia::value clamp(
    const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if (val.valid() && min.valid() && max.valid())
    return ossia::apply_nonnull(
        apply_ternary_fun_visitor<clamp_functor>{}, val.v, min.v, max.v);
  return val;
}

#if defined(FAST_COMPILES)
ossia::value
clamp(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  return clamp(val, min, max);
}
#else
ossia::value
clamp(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if (val.valid() && min.valid() && max.valid())
    return ossia::apply_nonnull(
        apply_ternary_fun_visitor<clamp_functor>{}, ossia::move(val).v, min.v,
        max.v);
  return val;
}
#endif
}
