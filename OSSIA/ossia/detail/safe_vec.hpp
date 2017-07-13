#pragma once
#include <ossia/detail/algorithms.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
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
    impl.push_back(t);
  }

  void remove_all(T t)
  {
    boost::remove_erase(impl, t);
  }

  std::vector<T> copy()
  {
    return impl;
  }

private:
  std::vector<T> impl;
};
}
