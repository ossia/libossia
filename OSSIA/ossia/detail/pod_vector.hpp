#pragma once
#include <vector>
#include <cinttypes>
#include <cstddef>
#include <cstdlib>
namespace ossia
{

template <class T>
struct pod_allocator
{
    static_assert(std::is_pod_v<T>, "can only be used with POD types");
    static_assert(alignof(T) <= alignof(std::max_align_t), "type must not have specific alignment requirements");

    using value_type = T;

    auto allocate(std::size_t num)
    {
      return (T*) std::malloc(sizeof(T) * num);
    }

    void deallocate(T* p, std::size_t)
    {
      std::free(p);
    }
};

template<typename T>
using pod_vector = std::vector<T, pod_allocator<T>>;

using int_vector = pod_vector<int>;
using float_vector = pod_vector<float>;
using double_vector = pod_vector<double>;
}
