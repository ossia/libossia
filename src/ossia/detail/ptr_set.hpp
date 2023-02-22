#pragma once
#include <ossia/detail/hash_map.hpp>

namespace ossia
{

/*
* Efficient hash maps for pointers
*/
template <typename T>
using ptr_set = ossia::hash_set<T, egur_hash, pointer_equal>;
template <typename T, typename V>
using ptr_map = ossia::hash_map<T, V, egur_hash, pointer_equal>;

}
