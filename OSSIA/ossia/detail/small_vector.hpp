#pragma once
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>

namespace ossia
{
template<typename T, std::size_t N>
using small_vector = boost::container::small_vector<T, N>;
template<typename T, std::size_t N>
using static_vector = boost::container::static_vector<T, N>;
}
