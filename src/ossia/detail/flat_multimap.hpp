#pragma once
#include <boost/container/flat_map.hpp>

namespace ossia
{
template <typename K, typename V>
using flat_multimap = boost::container::flat_multimap<K, V>;
}
