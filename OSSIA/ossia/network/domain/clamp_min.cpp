#include "domain_base.hpp"
#include <ossia/network/domain/detail/clamp_visitors.hpp>

namespace ossia
{
struct clamp_min_functor
{
  template<typename... T>
  static OSSIA_INLINE auto compute(T&&... args) { return ossia::clamp_min(std::forward<T>(args)...); }
};

ossia::value clamp_min(const ossia::value& val, const ossia::value& min)
{
  if(val.valid() && min.valid())
    return ossia::apply(apply_binary_fun_visitor<clamp_min_functor>{}, val.v, min.v);
  return val;
}

#if defined(FAST_COMPILES)
ossia::value clamp_min(ossia::value&& val, const ossia::value& min)
{
  return clamp_min(val, min);
}
#else
ossia::value clamp_min(ossia::value&& val, const ossia::value& min)
{
  if(val.valid() && min.valid())
    return ossia::apply(apply_binary_fun_visitor<clamp_min_functor>{}, ossia::move(val).v, min.v);
  return val;
}
#endif
}
