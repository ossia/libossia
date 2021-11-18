#pragma once
#include <cinttypes>
#include <cstddef>
#include <cstdlib>
#include <vector>

#include <type_traits>
namespace ossia
{
#if defined(_MSC_VER) && defined(_DEBUG)
template <class T>
struct pod_allocator
{
  using value_type = T;

  template <typename... Args>
  explicit pod_allocator(Args&&...) noexcept
  {
  }

  pod_allocator() noexcept = default;
  pod_allocator(const pod_allocator&) noexcept = default;
  pod_allocator(pod_allocator&&) noexcept = default;
  pod_allocator& operator=(const pod_allocator&) noexcept = default;
  pod_allocator& operator=(pod_allocator&&) noexcept = default;

  static inline T* allocate(std::size_t num) noexcept
  {
    return new T[num];
  }

  static inline void deallocate(T* p, std::size_t) noexcept
  {
    delete[] p;
  }

  friend inline bool operator==(const pod_allocator& lhs, const pod_allocator& rhs) noexcept
  {
    return true;
  }
  friend inline bool operator!=(const pod_allocator& lhs, const pod_allocator& rhs) noexcept
  {
    return false;
  }
};

template <class T, std::size_t Align>
using aligned_pod_allocator = pod_allocator<T>;

#else
template <class T>
struct pod_allocator
{
  using value_type = T;

  pod_allocator() noexcept = default;
  pod_allocator(const pod_allocator&) noexcept = default;
  pod_allocator(pod_allocator&&) noexcept = default;
  pod_allocator& operator=(const pod_allocator&) noexcept = default;
  pod_allocator& operator=(pod_allocator&&) noexcept = default;

  static inline T* allocate(std::size_t num) noexcept
  {
    static_assert(std::is_standard_layout_v<T> && std::is_trivial_v<T>, "can only be used with POD types");
    static_assert(
        alignof(T) <= alignof(std::max_align_t),
        "type must not have specific alignment requirements");

    return (T*)std::malloc(sizeof(T) * num);
  }

  static inline void deallocate(T* p, std::size_t) noexcept
  {
    std::free(p);
  }

  friend inline bool operator==(pod_allocator lhs, pod_allocator rhs) noexcept
  {
    return true;
  }
  friend inline bool operator!=(pod_allocator lhs, pod_allocator rhs) noexcept
  {
    return false;
  }
};


template <class T, std::size_t Align>
struct aligned_pod_allocator
{
  template <class U>
  struct rebind {
    using other = aligned_pod_allocator<U, Align>;
  };

  using value_type = T;

  aligned_pod_allocator() noexcept = default;
  aligned_pod_allocator(const aligned_pod_allocator&) noexcept = default;
  aligned_pod_allocator(aligned_pod_allocator&&) noexcept = default;
  aligned_pod_allocator& operator=(const aligned_pod_allocator&) noexcept = default;
  aligned_pod_allocator& operator=(aligned_pod_allocator&&) noexcept = default;

  static inline T* allocate(std::size_t num) noexcept
  {
    static_assert(std::is_standard_layout_v<T> && std::is_trivial_v<T>, "can only be used with POD types");
    static_assert(
        alignof(T) <= alignof(std::max_align_t),
        "type must not have specific alignment requirements");

    if constexpr(Align != __STDCPP_DEFAULT_NEW_ALIGNMENT__)
    {
#if defined(_WIN32)
      return static_cast<T*>(::_aligned_malloc(sizeof(T) * num, Align));
#else
      void *p;
      posix_memalign(&p, Align, sizeof(T) * num);
      return static_cast<T*>(p);
#endif
    }
    else
    {
      return (T*)std::malloc(sizeof(T) * num);
    }
  }

  static inline void deallocate(T* p, std::size_t) noexcept
  {
#if defined(_WIN32)
    if constexpr(Align != __STDCPP_DEFAULT_NEW_ALIGNMENT__)
    {
      ::_aligned_free(p);
    }
    else
    {
      std::free(p);
    }
#else
    std::free(p);
#endif
  }

  friend inline bool operator==(aligned_pod_allocator lhs, aligned_pod_allocator rhs) noexcept
  {
    return true;
  }
  friend inline bool operator!=(aligned_pod_allocator lhs, aligned_pod_allocator rhs) noexcept
  {
    return false;
  }
};
#endif

template<typename T>
struct pod_allocator_avx2 : aligned_pod_allocator<T, 32> {
  template <class U>
  struct rebind {
    using other = pod_allocator_avx2<U>;
  };
  using aligned_pod_allocator<T, 32>::aligned_pod_allocator;
};

template <typename T>
using pod_vector = std::vector<T, pod_allocator_avx2<T>>;

using int_vector = pod_vector<int>;
using float_vector = pod_vector<float>;
using double_vector = pod_vector<double>;
}
