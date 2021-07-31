#pragma once
#include <ossia/detail/flat_map.hpp>
#include <ossia/detail/small_flat_map.hpp>

namespace ossia
{
template <typename T, typename Compare = std::less<void>>
using timed_vec = ossia::flat_map<int64_t, T, Compare>;

template <typename T, std::size_t N, typename Compare = std::less<void>>
using small_timed_vec = ossia::small_flat_map<int64_t, T, N, Compare>;
};
