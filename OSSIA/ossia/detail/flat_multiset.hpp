#pragma once
#include <flat_multiset.hpp>

namespace ossia
{
template<typename K, typename Compare = std::less<void>>
using flat_multiset = fc::vector_multiset<K, Compare>;
}
