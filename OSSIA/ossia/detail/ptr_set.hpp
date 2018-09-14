#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/hash.hpp>
#include <ossia/detail/math.hpp>

#include <hopscotch_map.h>
#include <hopscotch_set.h>

#include <type_traits>
namespace ossia
{

template <typename T>
struct PointerPredicate
{
  using is_transparent = std::true_type;
  bool operator()(const T* lhs, const T* rhs) const noexcept
  {
    return lhs == rhs;
  }
  bool operator()(const std::shared_ptr<T>& lhs, const T* rhs) const noexcept
  {
    return lhs.get() == rhs;
  }
  bool operator()(const T* lhs, const std::shared_ptr<T>& rhs) const noexcept
  {
    return lhs == rhs.get();
  }
  bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs)
      const noexcept
  {
    return lhs == rhs;
  }
};
template <typename T>
using ptr_set = tsl::hopscotch_set<T, EgurHash<std::remove_pointer_t<T>>>;
template <typename T, typename V>
using ptr_map = tsl::hopscotch_map<T, V, EgurHash<std::remove_pointer_t<T>>>;
template <typename T>
using shared_ptr_set
    = tsl::hopscotch_set<std::shared_ptr<T>, EgurHash<T>, PointerPredicate<T>>;
template <typename T, typename V>
using shared_ptr_map = tsl::hopscotch_map<
    std::shared_ptr<T>, V, EgurHash<T>, PointerPredicate<T>>;
}
