#pragma once
#include <boost/preprocessor/repetition/repeat.hpp>

#include <cassert>
#include <tuple>
#include <type_traits>
// Taken from
// https://www.reddit.com/r/cpp/comments/4yp7fv/c17_structured_bindings_convert_struct_to_a_tuple/

namespace ossia
{
namespace detail
{
struct any_type
{
  template <class T>
  constexpr operator T();
};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
template <class T, std::size_t... I>
decltype(void(T{(I, std::declval<any_type>())...}), std::true_type{})
    test_is_braces_constructible_n(std::index_sequence<I...>);
#pragma clang diagnostic pop
template <class, class...>
std::false_type test_is_braces_constructible_n(...);
template <class T, std::size_t N>
using is_braces_constructible_n
    = decltype(test_is_braces_constructible_n<T>(std::make_index_sequence<N>{}));

template <class T, std::size_t L = 0u, std::size_t R = sizeof(T) + 1u>
constexpr std::size_t to_tuple_size_f()
{
  constexpr std::size_t M = (L + R) / 2u;
  if constexpr(M == 0)
    return std::is_empty<T>{} ? 0u : throw "Unable to determine number of elements";
  else if constexpr(L == M)
    return M;
  else if constexpr(is_braces_constructible_n<T, M>{})
    return to_tuple_size_f<T, M, R>();
  else
    return to_tuple_size_f<T, L, M>();
}
template <class T>
using to_tuple_size = std::integral_constant<std::size_t, to_tuple_size_f<T>()>;

#ifndef TO_TUPLE_MAX
#define TO_TUPLE_MAX 5
#endif

template <class T>
auto to_tuple_impl(T&&, std::integral_constant<std::size_t, 0>) noexcept
{
  return std::make_tuple();
}

#define TO_TUPLE_P(Z, N, _) , p##N
#define TO_TUPLE_SPECIALIZATION(Z, N, _)                                              \
  template <class T>                                                                  \
  auto to_tuple_impl(T&& object, std::integral_constant<std::size_t, N + 1>) noexcept \
  {                                                                                   \
    auto&& [p BOOST_PP_REPEAT_##Z(N, TO_TUPLE_P, nil)] = object;                      \
    return std::make_tuple(p BOOST_PP_REPEAT_##Z(N, TO_TUPLE_P, nil));                \
  }
BOOST_PP_REPEAT(TO_TUPLE_MAX, TO_TUPLE_SPECIALIZATION, nil)
#undef TO_TUPLE_SPECIALIZATION
#undef TO_TUPLE_P

template <
    class T, class = struct current_value, std::size_t = TO_TUPLE_MAX,
    class = struct required_value, std::size_t N>
auto to_tuple_impl(T&&, std::integral_constant<std::size_t, N>) noexcept
{
  static_assert(N <= TO_TUPLE_MAX, "Please increase TO_TUPLE_MAX");
}
}

template <
    class T, class = std::enable_if_t<
                 std::is_class<T>::value && std::is_standard_layout<T>::value>>
auto to_tuple(T&& object) noexcept
{
  return detail::to_tuple_impl(
      std::forward<T>(object), detail::to_tuple_size<std::decay_t<T>>{});
}
}
