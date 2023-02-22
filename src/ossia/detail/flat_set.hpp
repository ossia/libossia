#pragma once
#include <boost/container/flat_set.hpp>

namespace ossia
{
template <typename K, typename Comparator = std::less<K>>
using flat_set = boost::container::flat_set<K, Comparator>;
}
