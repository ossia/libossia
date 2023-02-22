#pragma once
#include <boost/container/flat_set.hpp>

namespace ossia
{
template <typename K, typename Compare = std::less<void>>
using flat_multiset = boost::container::flat_multiset<K, Compare>;
}
