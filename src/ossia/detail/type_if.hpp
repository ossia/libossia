#pragma once
#include <utility>

namespace ossia
{

template <typename T, bool Predicate>
struct type_if;
template <typename T>
struct type_if<T, false>
{
  type_if() = default;
  type_if(const type_if&) = default;
  type_if(type_if&&) = default;
  type_if& operator=(const type_if&) = default;
  type_if& operator=(type_if&&) = default;

  template <typename U>
  type_if(U&&)
  {
  }
  template <typename U>
  T& operator=(U&& u) noexcept
  {
    return *this;
  }
};

template <typename T>
struct type_if<T, true>
{
  [[no_unique_address]] T value;

  type_if() = default;
  type_if(const type_if&) = default;
  type_if(type_if&&) = default;
  type_if& operator=(const type_if&) = default;
  type_if& operator=(type_if&&) = default;

  template <typename U>
  type_if(U&& other)
      : value{std::forward<U>(other)}
  {
  }

  operator const T&() const noexcept { return value; }
  operator T&() noexcept { return value; }
  operator T&&() && noexcept { return std::move(value); }

  template <typename U>
  T& operator=(U&& u) noexcept
  {
    return value = std::forward<U>(u);
  }
};
}
