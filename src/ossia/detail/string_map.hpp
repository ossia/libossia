#pragma once
#include <ossia/detail/hash_map.hpp>

/**
 * \file string_map.hpp
 *
 * Efficient hash maps for string and string_view
 */
namespace ossia
{
template <typename Value>
using string_map = ossia::hash_map<std::string, Value, string_hash, string_equal>;
template <typename Value>
using string_view_map
    = ossia::hash_map<std::string_view, Value, string_hash, string_equal>;
}
