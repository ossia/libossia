#pragma once
#include <ossia/detail/config.hpp>
#include <mutex>
#if !defined(__APPLE__)
#include <shared_mutex>
#endif
namespace ossia
{
#if !defined(__APPLE__)
using mutex_t = std::mutex;
using lock_t = std::lock_guard<mutex_t>;

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
