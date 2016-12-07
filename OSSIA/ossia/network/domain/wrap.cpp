#include "domain_base.hpp"
#include <ossia/network/domain/detail/clamp_visitors.hpp>

namespace ossia
{
struct wrap_functor
{
  template<typename... T>
  static OSSIA_INLINE auto compute(T&&... args) { return ossia::wrap(std::forward<T>(args)...); }
};

ossia::value wrap(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<wrap_functor>{}, val.v, min.v, max.v);
  return val;
}

#if defined(FAST_COMPILES)
ossia::value wrap(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  return wrap(val, min, max);
}
#else
ossia::value wrap(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<wrap_functor>{}, ossia::move(val).v, min.v, max.v);
  return val;
}
#endif
}
