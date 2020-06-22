#pragma once

#include <ossia/detail/config.hpp>
#include <optional>
namespace ossia
{
template <typename Opt, typename Arg>
auto get_value_or(const Opt& opt, Arg&& arg)
{
  return opt.value_or(std::forward<Arg>(arg));
}
}

namespace ossia
{
using none_t = decltype(std::nullopt);
}
