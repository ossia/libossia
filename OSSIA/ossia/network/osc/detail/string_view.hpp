#pragma once
#include <functional>
#include <oscpack/osc/SmallString.h>
#include <type_traits>
#include <ossia/detail/string_view.hpp>

namespace ossia
{
template <typename T, typename Enable = void>
struct param_traits
{
};

template <typename T>
struct param_traits<T, std::enable_if_t<std::is_same<T, std::string>::value>>
{
  using type = const T&;
};

template <typename T>
struct param_traits<T,
                    std::enable_if_t<std::is_same<T, ossia::string_view>::value>>
{
  using type = T;
};

template <typename T>
struct param_traits<T,
                    std::enable_if_t<std::is_same<T, oscpack::small_string>::
                                         value>>
{
  using type = T;
};

template <typename T>
using param_t = typename param_traits<T>::type;
}
