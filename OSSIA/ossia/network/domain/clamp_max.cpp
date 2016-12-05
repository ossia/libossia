#include "domain_base.hpp"
#include <ossia/network/domain/detail/clamp_visitors.hpp>

namespace ossia
{
struct clamp_max_functor
{
  template<typename... T>
  static OSSIA_INLINE auto compute(T&&... args) { return ossia::clamp_max(std::forward<T>(args)...); }
};

ossia::value clamp_max(const ossia::value& val, const ossia::value& max)
{
  if(val.valid() && max.valid())
    return eggs::variants::apply(apply_binary_fun_visitor<clamp_max_functor>{}, val.v, max.v);
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
  if(val.valid() && max.valid())
    return eggs::variants::apply(apply_binary_fun_visitor<clamp_max_functor>{}, ossia::move(val).v, max.v);
  return val;
}
#endif
}
