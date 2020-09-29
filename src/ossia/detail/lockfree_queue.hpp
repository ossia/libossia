#pragma once

// TSAN seems to have some trouble with ReaderWriterQueue...
#if defined(__has_feature)
  #if __has_feature(thread_sanitizer)
    #include <concurrentqueue.h>
    namespace ossia {
      template<typename T, size_t MAX_BLOCK_SIZE = 512>
      using spsc_queue = moodycamel::ConcurrentQueue<T>;
    }
  #else
    #include <readerwriterqueue.h>
    namespace ossia {
      template<typename T, size_t MAX_BLOCK_SIZE = 512>
      using spsc_queue = moodycamel::ReaderWriterQueue<T, MAX_BLOCK_SIZE>;
    }
  #endif
#else
#include <readerwriterqueue.h>
namespace ossia {
  template<typename T, size_t MAX_BLOCK_SIZE = 512>
  using spsc_queue = moodycamel::ReaderWriterQueue<T, MAX_BLOCK_SIZE>;
}
#endif
