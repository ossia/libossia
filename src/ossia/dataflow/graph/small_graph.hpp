#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/small_vector.hpp>

#include <boost/graph/adjacency_list.hpp>

#if defined(OSSIA_SMALL_VECTOR)
namespace boost
{

struct smallvecS
{
};

namespace detail
{

template <class Directed>
struct is_random_access;
template <>
struct is_random_access<smallvecS>
{
  enum
  {
    value = true
  };
  typedef mpl::true_ type;
};
}

template <class ValueType>
struct container_gen<smallvecS, ValueType>
{
  // using Allocator =  typename Alloc::template rebind<ValueType>::other Allocator;

  // Needed because tests allocate graphs on the stack and that's too much
#if defined(OSSIA_TESTING) || defined(OSSIA_FREESTANDING)
  static constexpr std::size_t small_vec_size = 16;
#else
  static constexpr std::size_t small_vec_size = 1024;
#endif
  typedef ossia::small_vector<ValueType, small_vec_size> type;
};

template <>
struct parallel_edge_traits<smallvecS>
{
  typedef allow_parallel_edge_tag type;
};

namespace container
{
template <class T, std::size_t N>
graph_detail::vector_tag container_category(const ossia::small_vector<T, N>&)
{
  return graph_detail::vector_tag();
}

template <class T, std::size_t N>
graph_detail::unstable_tag iterator_stability(const ossia::small_vector<T, N>&)
{
  return graph_detail::unstable_tag();
}
}

namespace graph_detail
{
using container::container_category;
using container::iterator_stability;

template <class T, std::size_t N>
struct container_traits<ossia::small_vector<T, N>>
{
  typedef vector_tag category;
  typedef unstable_tag iterator_stability;
};
}

using container::container_category;
using container::iterator_stability;

}
#else
namespace boost
{
using smallvecS = vecS;
}
#endif
