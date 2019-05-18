#pragma once
#include <flat/flat_map.hpp>

namespace ossia
{
template <typename K, typename V>
using flat_map = fc::vector_map<K, V>;
}
