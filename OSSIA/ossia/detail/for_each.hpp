#pragma once

namespace ossia
{
/**
 * \file ForEachType
 * \brief Algorithms to iterate on a type list.
 *
 * There are two variants :
 *
 * `for_each_type` and `for_each_type_if`.
 *
 * Example:
 *
 * \code
 * struct foo {
 *  void operator()(int) { ... }
 *  void operator()(float) { ... }
 *  void operator()(MyType) { ... }
 * };
 * for_each_type<int, float, MyType>(foo{});
 * for_each_type<int, float, MyType>(foo{});
 * \endcode
 *
 */
// Invokes the functor with every type

template<typename T>
struct type_tag
{
  using type = T;
};

template <template<typename...> typename T, typename... Args, typename F>
constexpr void for_each(T<Args...>, F f)
{
  (f.template operator ()<Args>(), ...);
}

template <template<typename...> typename T, typename... Args, typename F>
constexpr void for_each_tagged(T<Args...>, F f)
{
  (f(type_tag<Args>{}), ...);
}

template <typename... Args, typename F> constexpr void for_each_type(F f)
{
  (f.template operator ()<Args>(), ...);
}

template <typename... Args, typename F>
constexpr void for_each_type_tagged(F f)
{
  (f(type_tag<Args>{}), ...);
}

// Invokes the functor with every type, this code generation is done at compile
// time
template <typename... Args, typename F>
constexpr void for_each_type_if(F&& f)
{
  (f.template operator ()<Args>() || ...);
}

template <typename... Args, typename F>
constexpr void for_each_type_if_tagged(F&& f)
{
  (f(type_tag<Args>{}) || ...);
}
}
