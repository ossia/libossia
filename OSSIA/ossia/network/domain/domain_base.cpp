#include "domain_base.hpp"
#include <boost/container/small_vector.hpp>
#include <ossia/network/domain/detail/apply_domain.hpp>
#include <ossia/network/domain/detail/clamp_visitors.hpp>
#include <ossia/network/domain/detail/min_max.hpp>
#include <ossia/network/domain/domain.hpp>
#if defined(FAST_COMPILES)
namespace ossia
{
template<typename T>
OSSIA_INLINE auto&
move(const T& t)
{
  return t;
}
}
#else
namespace ossia
{
template<typename T>
constexpr typename std::remove_reference<T>::type&&
move(T&& t) noexcept
{
  return static_cast<typename std::remove_reference<T>::type&&>(t);
}
}
#endif
namespace ossia
{
struct clamp_functor
{
  template<typename... T>
  OSSIA_INLINE auto operator()(T&&... args) { return ossia::clamp(std::forward<T>(args)...); }
};
struct clamp_min_functor
{
  template<typename... T>
  OSSIA_INLINE auto operator()(T&&... args) { return ossia::clamp_min(std::forward<T>(args)...); }
};
struct clamp_max_functor
{
  template<typename... T>
  OSSIA_INLINE auto operator()(T&&... args) { return ossia::clamp_max(std::forward<T>(args)...); }
};
struct wrap_functor
{
  template<typename... T>
  OSSIA_INLINE auto operator()(T&&... args) { return ossia::wrap(std::forward<T>(args)...); }
};
struct fold_functor
{
  template<typename... T>
  OSSIA_INLINE auto operator()(T&&... args) { return ossia::fold(std::forward<T>(args)...); }
};

/// Const-reference overloads
ossia::value clamp(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<clamp_functor>{}, val.v, min.v, max.v);
  return val;
}

ossia::value clamp_min(const ossia::value& val, const ossia::value& min)
{
  if(val.valid() && min.valid())
    return eggs::variants::apply(apply_binary_fun_visitor<clamp_min_functor>{}, val.v, min.v);
  return val;
}

ossia::value clamp_max(const ossia::value& val, const ossia::value& max)
{
  if(val.valid() && max.valid())
    return eggs::variants::apply(apply_binary_fun_visitor<clamp_max_functor>{}, val.v, max.v);
  return val;
}

ossia::value wrap(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<wrap_functor>{}, val.v, min.v, max.v);
  return val;
}

ossia::value fold(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<fold_functor>{}, val.v, min.v, max.v);
  return val;
}

/// Move-overloads
#if defined(FAST_COMPILES)
ossia::value clamp(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  return clamp(val, min, max);
}

ossia::value clamp_min(ossia::value&& val, const ossia::value& min)
{
  return clamp_min(val, min);
}

ossia::value clamp_max(ossia::value&& val, const ossia::value& max)
{
  return clamp_max(val, min);
}

ossia::value wrap(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  return wrap(val, min);
}

ossia::value fold(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  return fold(val, min);
}
#else
ossia::value clamp(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<clamp_functor>{}, ossia::move(val).v, min.v, max.v);
  return val;
}

ossia::value clamp_min(ossia::value&& val, const ossia::value& min)
{
  if(val.valid() && min.valid())
    return eggs::variants::apply(apply_binary_fun_visitor<clamp_min_functor>{}, ossia::move(val).v, min.v);
  return val;
}

ossia::value clamp_max(ossia::value&& val, const ossia::value& max)
{
  if(val.valid() && max.valid())
    return eggs::variants::apply(apply_binary_fun_visitor<clamp_max_functor>{}, ossia::move(val).v, max.v);
  return val;
}

ossia::value wrap(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<wrap_functor>{}, ossia::move(val).v, min.v, max.v);
  return val;
}

ossia::value fold(ossia::value&& val, const ossia::value& min, const ossia::value& max)
{
  if(val.valid() && min.valid() && max.valid())
    return eggs::variants::apply(apply_ternary_fun_visitor<fold_functor>{}, ossia::move(val).v, min.v, max.v);
  return val;
}
#endif

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
operator==(const domain_base<Int>& lhs, const domain_base<Int>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Float>& lhs, const domain_base<Float>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Bool>& lhs, const domain_base<Bool>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Char>& lhs, const domain_base<Char>& rhs)
{
  return lhs.min == rhs.min && lhs.max == rhs.max && lhs.values == rhs.values;
}
bool
operator==(const domain_base<Tuple>& lhs, const domain_base<Tuple>& rhs)
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
operator==(const domain_base<String>& lhs, const domain_base<String>& rhs)
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
      return domain_base<Tuple>();
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
