#include "domain_base.hpp"
#include <ossia/network/domain/detail/clamp_visitors.hpp>

namespace ossia
{
struct fold_functor
{
  template<typename... T>
  static OSSIA_INLINE auto compute(T&&... args) { return ossia::fold(std::forward<T>(args)...); }
};

ossia::value fold(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<fold_functor>{}, val.v, min.v, max.v);
  return val;
}

#if defined(FAST_COMPILES)
ossia::value fold(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  return fold(val, min, max);
}
#else
ossia::value fold(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<fold_functor>{}, ossia::move(val).v, min.v, max.v);
  return val;
}
#endif
}
