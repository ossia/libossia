#pragma once
#include <hopscotch_set.h>
#include <hopscotch_map.h>
#include <ossia/detail/math.hpp>
#include <type_traits>
namespace ossia
{

// https://stackoverflow.com/q/20953390/1495627
template<typename T>
struct EgurHash
{
    static const constexpr size_t shift = constexpr_log2(1 + sizeof(T));

    size_t operator()(const T* val) const
    {
      return (size_t)(val) >> shift;
    }
    size_t operator()(const std::shared_ptr<T>& val) const
    {
      return (size_t)(val.get()) >> shift;
    }
};

template<typename T>
struct PointerPredicate
{
  using is_transparent = std::true_type;
  bool operator()(const T* lhs, const T* rhs) const
  {
    return lhs == rhs;
  }
  bool operator()(const std::shared_ptr<T>& lhs, const T* rhs) const
  {
    return lhs.get() == rhs;
  }
  bool operator()(const T* lhs, const std::shared_ptr<T>& rhs) const
  {
    return lhs == rhs.get();
  }
  bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const
  {
    return lhs == rhs;
  }
};
template<typename T>
using ptr_set = tsl::hopscotch_set<T, EgurHash<std::remove_pointer_t<T>>>;
template<typename T, typename V>
using ptr_map = tsl::hopscotch_map<T, V, EgurHash<std::remove_pointer_t<T>>>;
template<typename T>
using shared_ptr_set = tsl::hopscotch_set<
  std::shared_ptr<T>,
  EgurHash<T>,
  PointerPredicate<T>
>;
template<typename T, typename V>
using shared_ptr_map = tsl::hopscotch_map<
  std::shared_ptr<T>, V,
  EgurHash<T>,
  PointerPredicate<T>
>;

}
