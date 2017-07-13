#pragma once
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{

template <typename T>
struct domain_base;

template <typename Domain>
struct value_set_clamp
{
  const Domain& domain;
  template <typename U>
  value operator()(bounding_mode b, U&& val)
  {
    const auto& values = domain.values;
    if (b == bounding_mode::FREE || values.empty())
    {
      return std::forward<U>(val);
    }
    else
    {
      auto it = values.find(val);
      return (it != values.end()) ? ossia::value{std::forward<U>(val)}
                                  : ossia::value{};
    }
  }
};
}
