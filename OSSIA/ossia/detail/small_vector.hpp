#pragma once
#include <boost/container/small_vector.hpp>

namespace ossia
{
template<typename T, std::size_t N>
using small_vector = boost::container::small_vector<T, N>;
}
