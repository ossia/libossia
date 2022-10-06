#pragma once
#include <ossia/detail/config.hpp>

#include <mutex>
#if defined(OSSIA_SHARED_MUTEX_AVAILABLE)
#include <shared_mutex>
#endif

namespace ossia
{
#if defined(OSSIA_SHARED_MUTEX_AVAILABLE)
// https://stackoverflow.com/questions/69990339/why-is-stdmutex-so-much-worse-than-stdshared-mutex-in-visual-c
#if !defined(_MSC_VER)
using mutex_t = std::mutex;
using lock_t = std::lock_guard<mutex_t>;
#else
using mutex_t = std::shared_mutex;
using lock_t = std::lock_guard<mutex_t>;
#endif

using shared_mutex_t = std::shared_timed_mutex;
using write_lock_t = std::lock_guard<shared_mutex_t>;
using read_lock_t = std::shared_lock<shared_mutex_t>;
#else
using mutex_t = std::mutex;
using shared_mutex_t = std::mutex;
using lock_t = std::lock_guard<mutex_t>;
using write_lock_t = std::lock_guard<mutex_t>;
using read_lock_t = std::lock_guard<mutex_t>;
#endif
}

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x) // no-op
#endif

#define TS_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define TS_SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define TS_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define TS_PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define TS_ACQUIRED_BEFORE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define TS_ACQUIRED_AFTER(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define TS_REQUIRES(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define TS_REQUIRES_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define TS_ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define TS_ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define TS_RELEASE(...) THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define TS_RELEASE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TS_RELEASE_GENERIC(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_generic_capability(__VA_ARGS__))

#define TS_TRY_ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TS_TRY_ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define TS_EXCLUDES(...) THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define TS_ASSERT_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define TS_ASSERT_SHARED_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define TS_RETURN_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define TS_NO_THREAD_SAFETY_ANALYSIS \
  THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)
