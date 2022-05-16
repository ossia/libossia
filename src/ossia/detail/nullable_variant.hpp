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
OSSIA_MAXIMUM_INLINE constexpr bool operator==(nullable_variant_index lhs, nullable_variant_index rhs) noexcept
{ return lhs.value == rhs.value; }
OSSIA_MAXIMUM_INLINE constexpr bool operator!=(nullable_variant_index lhs, nullable_variant_index rhs) noexcept
{ return lhs.value != rhs.value; }
OSSIA_MAXIMUM_INLINE constexpr bool operator<(nullable_variant_index lhs, nullable_variant_index rhs) noexcept
{ return lhs.value < rhs.value; }

template<typename... Args>
struct nullable_variant : public mpark::variant<mpark::monostate, Args...>
{
  using base = typename mpark::variant<mpark::monostate, Args...>;
  using base::base;

  static constexpr nullable_variant_index npos{0};

  template<typename T>
  static constexpr nullable_variant_index index_of() noexcept {
    if constexpr(!boost::mp11::mp_contains<base, T>::value)
      return npos;
    else
      return {boost::mp11::mp_find<base, T>::value};
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

  template<typename T>
  OSSIA_MAXIMUM_INLINE constexpr T& get() noexcept {
    return mpark::get<T>(*this);
  }
  template<typename T>
  OSSIA_MAXIMUM_INLINE constexpr const T& get() const noexcept {
    return mpark::get<T>(*this);
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
struct get_variant_visitation_return_type;
template<typename F, typename A1, typename... A1s>
struct get_variant_visitation_return_type<F, ossia::nullable_variant<A1, A1s...>> {
  using type = std::invoke_result_t<F, A1&>;
};
template<typename F, typename A1, typename... A1s, typename A2, typename... A2s>
struct get_variant_visitation_return_type<F, ossia::nullable_variant<A1, A1s...>, ossia::nullable_variant<A2, A2s...>> {
    using type = std::invoke_result_t<F, A1&, A2&>;
};
template<typename F, typename A1, typename... A1s, typename A2, typename... A2s, typename A3, typename... A3s>
struct get_variant_visitation_return_type<F, ossia::nullable_variant<A1, A1s...>, ossia::nullable_variant<A2, A2s...>, ossia::nullable_variant<A3, A3s...>> {
    using type = std::invoke_result_t<F, A1&, A2&, A3&>;
};

template<typename F, typename ret>
struct apply_call_empty_wrapper {
  F&& visitor;
  ret bad() const {
    if constexpr(requires { visitor(); }) {
      return visitor();
    }
    else if constexpr(requires (ossia::monostate m) { visitor(m); }) {
      ossia::monostate m;
      return visitor(m);
    }
    else {
      static_assert(F::this_should_not_happen);
    }
  }
  template<typename T>
  ret operator()(T&& t) const { return visitor(std::forward<T>(t)); }
  ret operator()(const ossia::monostate&) const { return bad(); }
  ret operator()(ossia::monostate&) const { return bad(); }
  ret operator()(ossia::monostate&&) const { return bad(); }
};


template<typename F, typename Arg>
OSSIA_MAXIMUM_INLINE auto apply(F&& visitor, Arg&& variant)
  -> decltype(auto)
{
  using ret = typename get_variant_visitation_return_type<std::decay_t<F>, std::decay_t<Arg>>::type;
  return mpark::visit(
        apply_call_empty_wrapper<F, ret>{std::forward<F>(visitor)},
        variant);
}


template<typename F, typename ret>
struct apply_nonnull_wrapper {
  F&& visitor;
  template<typename T>
  ret operator()(T&& t) const { return visitor(std::forward<T>(t)); }
  ret operator()(const ossia::monostate&) const { throw std::runtime_error("apply_nonnull called on invalid variant"); }
  ret operator()(ossia::monostate&) const { throw std::runtime_error("apply_nonnull called on invalid variant"); }
  ret operator()(ossia::monostate&&) const { throw std::runtime_error("apply_nonnull called on invalid variant"); }
};

template<typename F, typename ret>
struct apply_nonnull_wrapper_2 {
    F&& visitor;

    template<typename U>
    ret operator()(const ossia::monostate& t, U&& u) const {
      throw std::runtime_error("apply_nonnull called on invalid variant");
    }
    template<typename T>
    ret operator()(T&& t, const ossia::monostate& u) const {
      throw std::runtime_error("apply_nonnull called on invalid variant");
    }

    ret operator()(const ossia::monostate& t, const ossia::monostate& u) const {
      throw std::runtime_error("apply_nonnull called on invalid variant");
    }
    template<typename T, typename U>
    ret operator()(T&& t, U&& u) const
    {
      using type1 = std::remove_const_t<std::decay_t<T>>;
      using type2 = std::remove_const_t<std::decay_t<U>>;
      if constexpr(!std::is_same_v<type1, ossia::monostate> && !std::is_same_v<type2, ossia::monostate>
                   && !std::is_same_v<type1, const ossia::monostate> && !std::is_same_v<type2, const ossia::monostate>)
      {
        return visitor(std::forward<T>(t), std::forward<U>(u));
      }
      else
      {
        throw std::runtime_error("apply_nonnull called on invalid variant");
        if constexpr(std::is_same_v<ret, void>)
          return;
        else
          return ret{};
      }
    }
};
template<typename F, typename ret>
struct apply_nonnull_wrapper_3 {
    F&& visitor;
    template<typename T, typename U, typename V>
    ret operator()(T&& t, U&& u, V&& v) const
    {
      using type1 = std::remove_const_t<std::decay_t<T>>;
      using type2 = std::remove_const_t<std::decay_t<U>>;
      using type3 = std::remove_const_t<std::decay_t<V>>;
      if constexpr(!std::is_same_v<type1, ossia::monostate> && !std::is_same_v<type2, ossia::monostate> && !std::is_same_v<type3, ossia::monostate>)
      {
        return visitor(std::forward<T>(t), std::forward<U>(u), std::forward<V>(v));
      }
      else
      {
        throw std::runtime_error("apply_nonnull called on invalid variant");
        if constexpr(std::is_same_v<ret, void>)
          return;
        else
          return ret{};
      }
    }
};

template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, ossia::nullable_variant<Args...>& variant)
-> decltype(auto)
{
  using ret = typename get_variant_visitation_return_type<std::decay_t<F>, ossia::nullable_variant<Args...>>::type;
  return mpark::visit(
        apply_nonnull_wrapper<F, ret>{std::forward<F>(visitor)},
        variant);
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, const ossia::nullable_variant<Args...>& variant)
-> decltype(auto)
{
  using ret = typename get_variant_visitation_return_type<std::decay_t<F>, ossia::nullable_variant<Args...>>::type;
  return mpark::visit(
        apply_nonnull_wrapper<F, ret>{std::forward<F>(visitor)},
        variant);
}
template<typename F, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, ossia::nullable_variant<Args...>&& variant)
-> decltype(auto)
{
  using ret = typename get_variant_visitation_return_type<std::decay_t<F>, ossia::nullable_variant<Args...>>::type;
  return mpark::visit(
        apply_nonnull_wrapper<F, ret>{std::forward<F>(visitor)},
        std::move(variant));
}
template<typename F, typename T, typename U>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, T&& v1, U&& v2)
-> decltype(auto)
{
  using ret = typename get_variant_visitation_return_type<std::decay_t<F>, std::decay_t<T>, std::decay_t<U>>::type;
  return mpark::visit(
        apply_nonnull_wrapper_2<F, ret>{std::forward<F>(visitor)},
        std::forward<T>(v1), std::forward<U>(v2));
}
template<typename F, typename T, typename U, typename V>
OSSIA_MAXIMUM_INLINE auto apply_nonnull(F&& visitor, T&& v1, U&& v2, V&& v3)
-> decltype(auto)
{
  using ret = typename get_variant_visitation_return_type<std::decay_t<F>, std::decay_t<T>, std::decay_t<U>, std::decay_t<V>>::type;
  return mpark::visit(
        apply_nonnull_wrapper_3<F, ret>{std::forward<F>(visitor)},
        std::forward<T>(v1), std::forward<U>(v2), std::forward<V>(v3));
}

template<typename F, typename Arg, typename... Args>
OSSIA_MAXIMUM_INLINE auto apply(F&& visitor, Arg&& arg, Args&&... rem)
  -> decltype(auto)
{
  return apply_nonnull(std::forward<F>(visitor), std::forward<Arg>(arg), std::forward<Args>(rem)...);
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
    if(lhs_idx != rhs.which())
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
    if(lhs_idx != rhs.which())
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
    if(lhs_idx < rhs.which())
      return true;
    else if(lhs_idx > rhs.which())
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
    if(lhs_idx > rhs.which())
      return true;
    else if(lhs_idx < rhs.which())
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
    if(lhs_idx < rhs.which())
      return true;
    else if(lhs_idx > rhs.which())
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
    if(lhs_idx > rhs.which())
      return true;
    else if(lhs_idx < rhs.which())
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
    if(lhs.which() != rhs_idx)
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
    if(lhs.which() != rhs_idx)
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
    if(lhs.which() < rhs_idx)
      return true;
    else if(lhs.which() > rhs_idx)
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
    if(lhs.which() > rhs_idx)
      return true;
    else if(lhs.which() < rhs_idx)
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
    if(lhs.which() < rhs_idx)
      return true;
    else if(lhs.which() > rhs_idx)
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
    if(lhs.which() > rhs_idx)
      return true;
    else if(lhs.which() < rhs_idx)
      return false;
    else
      return *lhs.template target<R>() >= rhs;
  }
}

}
