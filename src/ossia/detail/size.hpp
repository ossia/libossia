#pragma once

namespace ossia
{

template <typename T>
struct size;

template <template <typename...> typename T, typename... Args>
struct size<T<Args...>>
{
  static const constexpr int value = sizeof...(Args);
};
}
