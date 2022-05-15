#pragma once
#include <ossia/detail/config.hpp>
#include <ossia/detail/variant.hpp>
#include <mpark/variant.hpp>
#include <boost/mp11.hpp>
namespace ossia
{
struct nullable_variant_index {
  std::size_t value;
  OSSIA_MAXIMUM_INLINE constexpr bool valid() const noexcept
  { return value != 0; }
  OSSIA_MAXIMUM_INLINE constexpr std::size_t index() const noexcept
  { return value; }
  OSSIA_MAXIMUM_INLINE constexpr std::size_t to_std_index() const noexcept
  { return value - 1; }
};

template<typename... Args>
struct nullable_variant : public mpark::variant<mpark::monostate, Args...>
{
  using base = typename mpark::variant<mpark::monostate, Args...>;
  using base::base;

  static constexpr std::size_t npos = 0;

  template<typename T>
  static constexpr std::size_t index_of() noexcept {
    if constexpr(!boost::mp11::mp_contains<base, T>::value)
      return 0;
    else
      return boost::mp11::mp_find<base, T>::value;
  }

  OSSIA_MAXIMUM_INLINE constexpr operator bool() const noexcept {
    return this->index() != 0;
  }

  OSSIA_MAXIMUM_INLINE constexpr nullable_variant_index which() const noexcept {
    return nullable_variant_index{this->index()};
  }

  template<typename T>
  OSSIA_MAXIMUM_INLINE constexpr T* target() noexcept {
    return mpark::get_if<T>(this);
  }
  template<typename T>
  OSSIA_MAXIMUM_INLINE constexpr const T* target() const noexcept {
    return mpark::get_if<T>(this);
  }

  // FIXME is this safe
  [[deprecated]]
  OSSIA_MAXIMUM_INLINE constexpr void* target() noexcept {
    return this;
  }

  [[deprecated]]
  OSSIA_MAXIMUM_INLINE constexpr const void* target() const noexcept {
    return this;
  }
};

/*
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto visit(F&& visitor, Args&&... variants)
  -> decltype(auto)
{
  return mpark::visit(visitor, static_cast<Args&&>(variants)...);
}
*/
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply(F&& visitor, ossia::nullable_variant<Args...>& variant)
  -> decltype(auto)
{
  return mpark::visit(visitor, variant);
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply(F&& visitor, const ossia::nullable_variant<Args...>& variant)
  -> decltype(auto)
{
  return mpark::visit(visitor, variant);
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply(F&& visitor, ossia::nullable_variant<Args...>&& variant)
  -> decltype(auto)
{
  return mpark::visit(visitor, std::move(variant));
}

template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, ossia::nullable_variant<Args...>& variant)
-> decltype(auto)
{
  if(variant)
  {
    return mpark::visit(visitor, variant);
  }
  else
  {
    throw std::runtime_error("apply_nonnull called on invalid variant");
  }
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, const ossia::nullable_variant<Args...>& variant)
-> decltype(auto)
{
  if(variant)
  {
    return mpark::visit(visitor, variant);
  }
  else
  {
    throw std::runtime_error("apply_nonnull called on invalid variant");
  }
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, ossia::nullable_variant<Args...>&& variant)
-> decltype(auto)
{
  if(variant)
  {
    return mpark::visit(visitor, std::move(variant));
  }
  else
  {
    throw std::runtime_error("apply_nonnull called on invalid variant");
  }
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, ossia::nullable_variant<Args...>& v1, ossia::nullable_variant<Args...>& v2)
-> decltype(auto)
{
  if(v1 && v2)
  {
    return mpark::visit(visitor, v1, v2);
  }
  else
  {
    throw std::runtime_error("apply_nonnull called on invalid variant");
  }
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, const ossia::nullable_variant<Args...>& v1, const ossia::nullable_variant<Args...>& v2)
-> decltype(auto)
{
  if(v1 && v2)
  {
    return mpark::visit(visitor, v1, v2);
  }
  else
  {
    throw std::runtime_error("apply_nonnull called on invalid variant");
  }
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, ossia::nullable_variant<Args...>&& v1, const ossia::nullable_variant<Args...>&& v2)
-> decltype(auto)
{
  if(v1 && v2)
  {
    return mpark::visit(visitor, std::move(v1), std::move(v2));
  }
  else
  {
    throw std::runtime_error("apply_nonnull called on invalid variant");
  }
}

template <typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator==(const nullable_variant<Ts...>& lhs, const nullable_variant<Ts...>& rhs) {
  return ((const typename nullable_variant<Ts...>::base&) lhs) == ((const typename nullable_variant<Ts...>::base&) rhs);
}
template <typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator!=(const nullable_variant<Ts...>& lhs, const nullable_variant<Ts...>& rhs) {
  return ((const typename nullable_variant<Ts...>::base&) lhs) != ((const typename nullable_variant<Ts...>::base&) rhs);
}
template <typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator<(const nullable_variant<Ts...>& lhs, const nullable_variant<Ts...>& rhs) {
  return ((const typename nullable_variant<Ts...>::base&) lhs) < ((const typename nullable_variant<Ts...>::base&) rhs);
}
template <typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator>(const nullable_variant<Ts...>& lhs, const nullable_variant<Ts...>& rhs) {
  return ((const typename nullable_variant<Ts...>::base&) lhs) > ((const typename nullable_variant<Ts...>::base&) rhs);
}
template <typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator<=(const nullable_variant<Ts...>& lhs, const nullable_variant<Ts...>& rhs) {
  return ((const typename nullable_variant<Ts...>::base&) lhs) <= ((const typename nullable_variant<Ts...>::base&) rhs);
}
template <typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator>=(const nullable_variant<Ts...>& lhs, const nullable_variant<Ts...>& rhs) {
  return ((const typename nullable_variant<Ts...>::base&) lhs) >= ((const typename nullable_variant<Ts...>::base&) rhs);
}

template <typename L, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator==(const L& lhs, const nullable_variant<Ts...>& rhs) {
  constexpr auto lhs_idx = nullable_variant<Ts...>::template index_of<L>();
  static_assert(lhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs_idx != rhs.index())
      return false;
    else
      return lhs == *rhs.template target<L>();
  }
}
template <typename L, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator!=(const L& lhs, const nullable_variant<Ts...>& rhs) {
  constexpr auto lhs_idx = nullable_variant<Ts...>::template index_of<L>();
  static_assert(lhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs_idx != rhs.index())
      return true;
    else
      return lhs != *rhs.template target<L>();
  }
}

template <typename L, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator<(const L& lhs, const nullable_variant<Ts...>& rhs) {
  constexpr auto lhs_idx = nullable_variant<Ts...>::template index_of<L>();
  static_assert(lhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs_idx < rhs.index())
      return true;
    else if(lhs_idx > rhs.index())
      return false;
    else
      return lhs < *rhs.template target<L>();
  }
}

template <typename L, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator>(const L& lhs, const nullable_variant<Ts...>& rhs) {
  constexpr auto lhs_idx = nullable_variant<Ts...>::template index_of<L>();
  static_assert(lhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs_idx > rhs.index())
      return true;
    else if(lhs_idx < rhs.index())
      return false;
    else
      return lhs > *rhs.template target<L>();
  }
}
template <typename L, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator<=(const L& lhs, const nullable_variant<Ts...>& rhs) {
  constexpr auto lhs_idx = nullable_variant<Ts...>::template index_of<L>();
  static_assert(lhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs_idx < rhs.index())
      return true;
    else if(lhs_idx > rhs.index())
      return false;
    else
      return lhs <= *rhs.template target<L>();
  }
}
template <typename L, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator>=(const L& lhs, const nullable_variant<Ts...>& rhs) {
  constexpr auto lhs_idx = nullable_variant<Ts...>::template index_of<L>();
  static_assert(lhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs_idx > rhs.index())
      return true;
    else if(lhs_idx < rhs.index())
      return false;
    else
      return lhs >= *rhs.template target<L>();
  }
}
template <typename R, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator==(const nullable_variant<Ts...>& lhs, const R& rhs) {
  constexpr auto rhs_idx = nullable_variant<Ts...>::template index_of<R>();
  static_assert(rhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs.index() != rhs_idx)
      return false;
    else
      return *lhs.template target<R>() == rhs;
  }
}

template <typename R, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator!=(const nullable_variant<Ts...>& lhs, const R& rhs) {
  constexpr auto rhs_idx = nullable_variant<Ts...>::template index_of<R>();
  static_assert(rhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs.index() != rhs_idx)
      return true;
    else
      return *lhs.template target<R>() != rhs;
  }
}

template <typename R, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator<(const nullable_variant<Ts...>& lhs, const R& rhs) {
  constexpr auto rhs_idx = nullable_variant<Ts...>::template index_of<R>();
  static_assert(rhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs.index() < rhs_idx)
      return true;
    else if(lhs.index() > rhs_idx)
      return false;
    else
      return *lhs.template target<R>() < rhs;
  }
}
template <typename R, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator>(const nullable_variant<Ts...>& lhs, const R& rhs) {
  constexpr auto rhs_idx = nullable_variant<Ts...>::template index_of<R>();
  static_assert(rhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs.index() > rhs_idx)
      return true;
    else if(lhs.index() < rhs_idx)
      return false;
    else
      return *lhs.template target<R>() > rhs;
  }
}
template <typename R, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator<=(const nullable_variant<Ts...>& lhs, const R& rhs) {
  constexpr auto rhs_idx = nullable_variant<Ts...>::template index_of<R>();
  static_assert(rhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs.index() < rhs_idx)
      return true;
    else if(lhs.index() > rhs_idx)
      return false;
    else
      return *lhs.template target<R>() <= rhs;
  }
}
template <typename R, typename... Ts>
OSSIA_MAXIMUM_INLINE constexpr bool operator>=(const nullable_variant<Ts...>& lhs, const R& rhs) {
  constexpr auto rhs_idx = nullable_variant<Ts...>::template index_of<R>();
  static_assert(rhs_idx != nullable_variant<Ts...>::npos);
  {
    if(lhs.index() > rhs_idx)
      return true;
    else if(lhs.index() < rhs_idx)
      return false;
    else
      return *lhs.template target<R>() >= rhs;
  }
}

}
