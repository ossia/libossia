#pragma once
#include <ossia/detail/config.hpp>
#include <ossia/detail/pod_vector.hpp>

#if !defined(OSSIA_NO_SMALLVECTOR)
#define OSSIA_SMALL_VECTOR
#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/version.hpp>

namespace ossia
{
template <typename T, std::size_t N>
using small_vector = boost::container::small_vector<T, N>;
template <typename T, std::size_t N>
using small_pod_vector = boost::container::small_vector<T, N, pod_allocator<T>>;
template <typename T, std::size_t N>
using static_vector = boost::container::static_vector<T, N>;
}
#else
namespace ossia
{
template <typename T, std::size_t N>
using small_vector = std::vector<T>;
template <typename T, std::size_t N>
using small_pod_vector = pod_vector<T>;
template <typename T, std::size_t N>
using static_vector = std::vector<T>;
}
#endif

#if BOOST_VERSION >= 107200
static_assert(noexcept(ossia::small_vector<int, 1>{}));
static_assert(noexcept(ossia::small_pod_vector<int, 1>{}));
static_assert(noexcept(ossia::static_vector<int, 1>{}));
#endif
