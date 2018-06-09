#pragma once
#include <flat_multimap.hpp>

namespace ossia
{
template<typename K, typename V>
using flat_multimap = fc::vector_multimap<K, V>;
}
