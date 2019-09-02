#pragma once
#include <ossia/detail/small_vector.hpp>

namespace ossia
{
template <typename T>
using value_vector = ossia::small_vector<T, 4>;
}
