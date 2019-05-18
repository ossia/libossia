#pragma once
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/ptr_set.hpp>

#include <vector>

namespace ossia
{
template <typename T>
class safe_vector
{
public:
  safe_vector()
  {
    static_assert(sizeof(T) <= 64, "use this only for small types");
  }

  safe_vector(const safe_vector&) = delete;
  safe_vector(safe_vector&&) = delete;
  safe_vector& operator=(const safe_vector&) = delete;
  safe_vector& operator=(safe_vector&&) = delete;

  bool contains(T t) noexcept
  {
    return ossia::contains(impl, t);
  }

  void clear() noexcept
  {
    impl.clear();
  }

  void push_back(T t)
  {
    impl.push_back(t);
  }

  void remove_all(T t)
  {
    ossia::remove_erase(impl, t);
  }

  void reserve(std::size_t N)
  {
    impl.reserve(N);
  }

  auto copy() const
  {
    return impl;
  }

  auto& reference() noexcept
  {
    return impl;
  }

  auto& reference() const noexcept
  {
    return impl;
  }

private:
  std::vector<T> impl;
};

template <typename T>
class safe_set
{
public:
  safe_set()
  {
    static_assert(sizeof(T) <= 64, "use this only for small types");
  }

  safe_set(const safe_set&) = delete;
  safe_set(safe_set&&) = delete;
  safe_set& operator=(const safe_set&) = delete;
  safe_set& operator=(safe_set&&) = delete;

  bool contains(T t)
  {
    return ossia::contains(impl, t);
  }

  void clear()
  {
    impl.clear();
  }

  void push_back(T t)
  {
    impl.insert(t);
  }

  void remove_all(T t)
  {
    impl.erase(t);
  }

  void reserve(std::size_t N)
  {
    impl.reserve(N);
  }

  auto copy() const
  {
    return impl;
  }

  auto& reference()
  {
    return impl;
  }

  auto& reference() const
  {
    return impl;
  }

  auto size() const
  {
    return impl.size();
  }

private:
  ossia::ptr_set<T> impl;
};
}
