#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/string_view.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>

/**
 * \file algorithms.hpp
 *
 * This header contains various range-style functions that mimic std::algorithm
 * functions.
 * This won't be necessary anymore when ranges are introduced in C++20
 * (hopefully).
 */
namespace ossia
{
template <typename Vector>
using iterator_t = typename std::remove_reference<Vector>::type::iterator;

template <typename Vector, typename Value>
auto find(Vector&& v, const Value& val) noexcept
{
  auto end = std::end(v);
  for(auto it = std::begin(v); it != end; ++it)
    if(*it == val)
      return it;
  return end;
}

template <typename Vector, typename Fun>
auto find_if(Vector&& v, Fun&& fun)
{
  auto end = std::end(v);
  for(auto it = std::begin(v); it != end; ++it)
    if(fun(*it))
      return it;
  return end;
}

template <typename Vector, typename Value>
auto* ptr_find(Vector&& v, const Value& val) noexcept
{
  if constexpr(requires { v.find(val) != v.end(); })
  {
    auto it = v.find(val);
    return it != v.end() ? &it->second : nullptr;
  }
  else
  {
    auto it = ossia::find(v, val);
    return it != std::end(v) ? &*it : nullptr;
  }
}

template <typename Vector, typename Fun>
auto* ptr_find_if(Vector&& v, Fun&& fun)
{
  auto it = ossia::find_if(v, fun);
  return it != std::end(v) ? &*it : nullptr;
}

template <typename Vector, typename Value>
bool contains(Vector&& v, const Value& val) noexcept
{
  return ossia::find(v, val) != std::end(v);
}

template <typename Vector, typename Value>
void remove_one(Vector&& v, const Value& val)
{
  auto it = ossia::find(v, val);
  if(it != v.end())
  {
    v.erase(it);
  }
}

template <typename Vector, typename Function>
void remove_one_if(Vector& v, const Function& val)
{
  auto it = ossia::find_if(v, val);
  if(it != v.end())
  {
    v.erase(it);
  }
}

template <typename Vector, typename Value>
void remove_erase(Vector& v, const Value& val)
{
  v.erase(std::remove(v.begin(), v.end(), val), v.end());
}

template <typename Vector, typename Function>
void remove_erase_if(Vector& v, const Function& val)
{
  v.erase(std::remove_if(v.begin(), v.end(), val), v.end());
}

template <typename Vector, typename Fun>
void erase_if(Vector& r, Fun f)
{
  for(auto it = std::begin(r); it != std::end(r);)
  {
    it = f(*it) ? r.erase(it) : ++it;
  }
}

template <typename Vector, typename Fun>
bool any_of(Vector&& v, Fun&& fun) noexcept
{
  for(auto it = std::begin(v); it != std::end(v); ++it)
    if(fun(*it))
      return true;
  return false;
}

template <typename Vector, typename Fun>
auto all_of(Vector&& v, Fun&& fun) noexcept
{
  for(auto it = std::begin(v); it != std::end(v); ++it)
    if(!fun(*it))
      return false;
  return true;
}

template <typename Vector, typename Fun>
bool none_of(Vector&& v, Fun&& fun) noexcept
{
  for(auto it = std::begin(v); it != std::end(v); ++it)
    if(fun(*it))
      return false;
  return true;
}

template <typename Vector, typename Fun>
auto remove_if(Vector&& v, Fun&& fun)
{
  return std::remove_if(std::begin(v), std::end(v), std::forward<Fun>(fun));
}

template <typename Vector, typename Fun>
auto count_if(Vector&& v, Fun&& fun)
{
  std::size_t count = 0;

  for(auto it = std::begin(v); it != std::end(v); ++it)
    if(fun(*it))
      ++count;

  return count;
}

template <typename Vector, typename Fun>
auto max_element(Vector&& v, Fun&& fun)
{
  return std::max_element(std::begin(v), std::end(v), std::forward<Fun>(fun));
}

template <typename Vector>
auto sort(Vector&& v)
{
  using value_type = typename std::remove_cvref_t<Vector>::value_type;
  return std::ranges::sort(v, std::less<value_type>{});
}

template <typename Vector, typename T>
auto fill(Vector&& v, const T& val)
{
  return std::fill(std::begin(v), std::end(v), val);
}

template <typename Vector>
auto unique(Vector&& v)
{
  return std::unique(std::begin(v), std::end(v));
}

template <typename Vector, typename Fun>
auto sort(Vector&& v, Fun&& fun)
{
  return std::ranges::sort(v, std::forward<Fun>(fun));
}

template <typename Vector, typename OutputIterator, typename Fun>
auto transform(Vector&& v, OutputIterator it, Fun f)
{
  return std::transform(v.begin(), v.end(), it, f);
}

template <typename Array1, typename Array2>
auto equal(const Array1& v, const Array2& v2) noexcept
{
  return std::equal(std::begin(v), std::end(v), std::begin(v2));
}

template <typename Vector1, typename Vector2>
void copy(const Vector1& source, Vector2& destination)
{
  destination.reserve(destination.size() + source.size());
  std::copy(source.begin(), source.end(), std::back_inserter(destination));
}

template <typename Vector1, typename Vector2, typename Pred>
void copy_if(const Vector1& source, Vector2& destination, Pred predicate)
{
  std::copy_if(source.begin(), source.end(), std::back_inserter(destination), predicate);
}

template <typename T, typename K>
auto last_before(T&& container, const K& k)
{
  auto it = container.upper_bound(k);
  if(it != container.begin())
  {
    std::advance(it, -1);
  }
  return it;
}

template <typename T, typename K>
auto find_key(T&& vec, const K& key) noexcept
{
  return std::find_if(
      vec.begin(), vec.end(), [&](const auto& elt) { return elt.first == key; });
}

template <typename T, typename K0, typename K1>
auto find_key(T&& vec, const K0& k0, const K1& k1) noexcept
{
  return std::find_if(vec.begin(), vec.end(), [&](const auto& elt) {
    using namespace std;
    return get<0>(elt) == k0 && get<1>(elt) == k1;
  });
}

template <std::size_t N>
struct num
{
  static const constexpr auto value = N;
};

template <class F, std::size_t... Is>
void for_each_in_range(F&& func, std::index_sequence<Is...>)
{
  (std::forward<F>(func)(num<Is>{}), ...);
}

template <std::size_t N, typename F>
void for_each_in_range(F&& func)
{
  for_each_in_range(std::forward<F>(func), std::make_index_sequence<N>());
}

namespace detail
{
template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N>
to_array_impl(T (&a)[N], std::index_sequence<I...>) noexcept
{
  return {{a[I]...}};
}
}

template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N]) noexcept
{
  return detail::to_array_impl(a, std::make_index_sequence<N>{});
}

template <typename... Args>
constexpr std::array<const char*, sizeof...(Args)> make_array(Args&&... args) noexcept
{
  return {args...};
}

template <typename T>
void remove_duplicates(T& vec)
{
  if(vec.size() <= 1)
    return;

  std::sort(vec.begin(), vec.end());
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

template <typename T, typename Comp>
void remove_duplicates(T& vec, Comp comparator)
{
  if(vec.size() <= 1)
    return;

  std::sort(vec.begin(), vec.end(), comparator);
  vec.erase(std::unique(vec.begin(), vec.end(), comparator), vec.end());
}

template <typename Container, typename K, typename Comp, typename... Args>
auto emplace_sorted(Container& vec, const K& k, Comp&& comp, Args&&... args)
    -> decltype(auto)
{
  auto it = std::lower_bound(vec.begin(), vec.end(), k, std::forward<Comp>(comp));
  return vec.emplace(it, std::forward<Args>(args)...);
}

template <
    typename D, template <typename, typename> typename S, typename T, typename Alloc>
auto insert_at_end(D& dest, S<T, Alloc>&& src)
{
  dest.insert(
      dest.end(), std::make_move_iterator(src.begin()),
      std::make_move_iterator(src.end()));
}

// https://stackoverflow.com/questions/45447361/how-to-move-certain-elements-of-stdvector-to-a-new-index-within-the-vector
template <typename T>
void change_item_position(T& v, size_t oldIndex, size_t newIndex)
{
  if(oldIndex > newIndex)
    std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
  else
    std::rotate(
        v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

template <typename Container, typename Item>
int index_in_container(Container& vec, Item i) noexcept
{
  auto it = std::find(vec.begin(), vec.end(), i);
  if(it != vec.end())
    return std::distance(vec.begin(), it);
  else
    return -1;
}
}
