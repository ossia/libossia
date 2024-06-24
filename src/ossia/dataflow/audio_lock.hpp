#pragma once

#include <ossia/detail/config.hpp>

#include <ossia/detail/audio_spin_mutex.hpp>
#include <ossia/detail/mutex.hpp>

#include <mutex>

#if defined(OSSIA_PARALLEL)
using ossia_audio_lock_t = std::unique_lock<ossia::audio_spin_mutex>;
#define OSSIA_EXEC_STATE_LOCK_READ(state) \
  ossia_audio_lock_t ossia_read_lock      \
  {                                       \
    (state).mutex                         \
  }
#define OSSIA_EXEC_STATE_LOCK_WRITE(state) \
  ossia_audio_lock_t ossia_write_lock      \
  {                                        \
    (state).mutex                          \
  }
#else
struct ossia_audio_lock_t
{
  void lock() { }
  void unlock() { }
};
#define OSSIA_EXEC_STATE_LOCK_READ(state) \
  ossia_audio_lock_t ossia_read_lock;     \
  (void)ossia_read_lock;
#define OSSIA_EXEC_STATE_LOCK_WRITE(state) \
  ossia_audio_lock_t ossia_write_lock;     \
  (void)ossia_write_lock;
#endif
