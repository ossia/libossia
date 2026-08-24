#pragma once
#include <boost/container/vector.hpp>
#include <boost/predef.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <type_traits>
#include <vector>

#if defined(_WIN32)
#include <malloc.h> // _aligned_malloc / _aligned_free
#endif

// posix_memalign: available on Linux (incl. Android), the BSDs, macOS / iOS and
// emscripten. Note that Apple does not define __unix__, hence the separate
// check.
#if defined(__linux__) || defined(__unix__) || defined(__unix) || defined(__APPLE__) \
    || defined(__EMSCRIPTEN__)
#define OSSIA_HAS_POSIX_MEMALIGN 1
#else
#define OSSIA_HAS_POSIX_MEMALIGN 0
#endif

// __builtin_assume_aligned: gcc >= 4.7 and clang. MSVC has no equivalent -
// std::assume_aligned is a no-op there too, so going through <memory> would not
// buy us anything.
#if defined(__has_builtin)
#if __has_builtin(__builtin_assume_aligned)
#define OSSIA_HAS_ASSUME_ALIGNED 1
#endif
#elif defined(__GNUC__)
#define OSSIA_HAS_ASSUME_ALIGNED 1
#endif
#if !defined(OSSIA_HAS_ASSUME_ALIGNED)
#define OSSIA_HAS_ASSUME_ALIGNED 0
#endif

// Same thing on the allocation function itself, so that the alignment is known
// at every call site and not only where ossia::assume_aligned is used.
#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(gnu::assume_aligned)
#define OSSIA_ASSUME_ALIGNED_RETURN(N) [[gnu::assume_aligned(N)]]
#endif
#endif
#if !defined(OSSIA_ASSUME_ALIGNED_RETURN)
#define OSSIA_ASSUME_ALIGNED_RETURN(N)
#endif

namespace ossia
{
//! Alignment guaranteed by pod_allocator_avx2, and thus by ossia::pod_vector
inline constexpr std::size_t pod_vector_alignment = 32;

/**
 * @brief Tell the optimizer that p is aligned on Align bytes.
 *
 * Undefined behaviour if it is not: only use on memory that came out of
 * aligned_pod_allocator, that is, anything stored in an ossia::pod_vector.
 * The compiler cannot infer this by itself as the alignment does not survive
 * the round-trip through the container member pointer.
 */
template <std::size_t Align = pod_vector_alignment, typename T>
[[nodiscard]] inline T* assume_aligned(T* p) noexcept
{
  static_assert(Align > 0 && (Align & (Align - 1)) == 0, "Align must be a power of two");
#if OSSIA_HAS_ASSUME_ALIGNED
  return static_cast<T*>(__builtin_assume_aligned(p, Align));
#else
  return p;
#endif
}

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

  static inline T* allocate(std::size_t num) noexcept { return new T[num]; }

  static inline void deallocate(T* p, std::size_t) noexcept { delete[] p; }

  friend inline bool
  operator==(const pod_allocator& lhs, const pod_allocator& rhs) noexcept
  {
    return true;
  }
  friend inline bool
  operator!=(const pod_allocator& lhs, const pod_allocator& rhs) noexcept
  {
    return false;
  }
};

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
    static_assert(
        std::is_standard_layout_v<T> && std::is_trivial_v<T>,
        "can only be used with POD types");
    static_assert(
        alignof(T) <= alignof(std::max_align_t),
        "type must not have specific alignment requirements");

    return (T*)std::malloc(sizeof(T) * num);
  }

  static inline void deallocate(T* p, std::size_t) noexcept { std::free(p); }

  friend inline bool operator==(pod_allocator lhs, pod_allocator rhs) noexcept
  {
    return true;
  }
  friend inline bool operator!=(pod_allocator lhs, pod_allocator rhs) noexcept
  {
    return false;
  }
};
#endif

/**
 * @brief An allocator which guarantees that the memory is aligned on Align.
 *
 * This must not be aliased away for any build configuration: users of
 * ossia::pod_vector are entitled to do aligned SIMD loads on its contents in
 * debug builds too.
 */
template <class T, std::size_t Align>
struct aligned_pod_allocator
{
  template <class U>
  struct rebind
  {
    using other = aligned_pod_allocator<U, Align>;
  };

  using value_type = T;

  static constexpr std::size_t alignment = Align;
  static_assert(Align > 0 && (Align & (Align - 1)) == 0, "Align must be a power of two");

  // Whether we have to go out of our way to obtain the requested alignment.
  // This is compared against alignof(std::max_align_t) and *not* against
  // __STDCPP_DEFAULT_NEW_ALIGNMENT__: the plain path below goes through
  // std::malloc, whose only guarantee is max_align_t, and there are targets
  // where operator new is more aligned than malloc.
  static constexpr bool needs_aligned_alloc = Align > alignof(std::max_align_t);

  aligned_pod_allocator() noexcept = default;
  aligned_pod_allocator(const aligned_pod_allocator&) noexcept = default;
  aligned_pod_allocator(aligned_pod_allocator&&) noexcept = default;
  aligned_pod_allocator& operator=(const aligned_pod_allocator&) noexcept = default;
  aligned_pod_allocator& operator=(aligned_pod_allocator&&) noexcept = default;

  template <class U>
  aligned_pod_allocator(const aligned_pod_allocator<U, Align>&) noexcept
  {
  }

  OSSIA_ASSUME_ALIGNED_RETURN(Align)
  static inline T* allocate(std::size_t num) noexcept
  {
    static_assert(
        std::is_standard_layout_v<T> && std::is_trivial_v<T>,
        "can only be used with POD types");
    static_assert(
        alignof(T) <= alignof(std::max_align_t),
        "type must not have specific alignment requirements");

    const std::size_t bytes = sizeof(T) * num;
    void* p{};

    if constexpr(needs_aligned_alloc)
    {
#if defined(_WIN32)
      p = ::_aligned_malloc(bytes, Align);
#elif OSSIA_HAS_POSIX_MEMALIGN
      // posix_memalign requires the alignment to be a multiple of sizeof(void*)
      static_assert(Align >= sizeof(void*));
      if(::posix_memalign(&p, Align, bytes) != 0)
        p = nullptr;
#elif defined(__cpp_aligned_new)
      p = ::operator new(bytes, std::align_val_t(Align), std::nothrow);
#else
      // Over-allocate, and store the offset to the real block in the byte just
      // before the pointer we return.
      static_assert(Align <= 255, "the offset has to fit in one byte");
      if(void* const root = std::malloc(bytes + Align))
      {
        const auto pb = std::uintptr_t(root);
        // Smallest aligned address strictly greater than pb, so that there is
        // always at least one byte in front of it to store the offset in.
        const auto pptr = (pb + Align) & ~std::uintptr_t(Align - 1);
        *((unsigned char*)pptr - 1) = (unsigned char)(pptr - pb);
        p = (void*)pptr;
      }
#endif
    }
    else
    {
      p = std::malloc(bytes);
    }

    return ossia::assume_aligned<Align>(static_cast<T*>(p));
  }

  static inline void deallocate(T* p, std::size_t) noexcept
  {
    if constexpr(needs_aligned_alloc)
    {
#if defined(_WIN32)
      ::_aligned_free(p);
#elif OSSIA_HAS_POSIX_MEMALIGN
      std::free(p);
#elif defined(__cpp_aligned_new)
      ::operator delete(p, std::align_val_t(Align), std::nothrow);
#else
      if(p)
      {
        const auto count = *((unsigned char*)p - 1);
        std::free((unsigned char*)p - count);
      }
#endif
    }
    else
    {
      std::free(p);
    }
  }

  friend inline bool
  operator==(aligned_pod_allocator lhs, aligned_pod_allocator rhs) noexcept
  {
    return true;
  }
  friend inline bool
  operator!=(aligned_pod_allocator lhs, aligned_pod_allocator rhs) noexcept
  {
    return false;
  }
};

template <typename T>
struct pod_allocator_avx2 : aligned_pod_allocator<T, pod_vector_alignment>
{
  template <class U>
  struct rebind
  {
    using other = pod_allocator_avx2<U>;
  };
  using aligned_pod_allocator<T, pod_vector_alignment>::aligned_pod_allocator;

  pod_allocator_avx2() noexcept = default;

  template <class U>
  pod_allocator_avx2(const pod_allocator_avx2<U>&) noexcept
  {
  }
};

template <typename T>
using pod_vector = boost::container::vector<T, pod_allocator_avx2<T>>;

using int_vector = pod_vector<int>;
using float_vector = pod_vector<float>;
using double_vector = pod_vector<double>;

//! v.data(), with the alignment guarantee made visible to the optimizer
template <typename T>
[[nodiscard]] inline T* aligned_data(pod_vector<T>& v) noexcept
{
  return ossia::assume_aligned<pod_vector_alignment>(v.data());
}
template <typename T>
[[nodiscard]] inline const T* aligned_data(const pod_vector<T>& v) noexcept
{
  return ossia::assume_aligned<pod_vector_alignment>(v.data());
}
}
