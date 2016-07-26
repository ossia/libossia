#pragma once
#include <algorithm>
#include <type_traits>

namespace OSSIA
{
template<typename Vector>
using iterator_t = typename std::remove_reference<Vector>::type::iterator;

template<typename Vector, typename Value>
auto find(Vector&& v, const Value& val)
{
  return std::find(std::begin(v), std::end(v), val);
}

template<typename Vector, typename Fun>
auto find_if(Vector&& v, Fun fun)
{
  return std::find_if(std::begin(v), std::end(v), fun);
}

template<typename Vector, typename Value>
bool contains(Vector&& v, const Value& val)
{
  return find(v, val) != std::end(v);
}

template<typename Vector, typename Value>
void remove_one(Vector&& v, const Value& val)
{
  auto it = find(v, val);
  if(it != v.end())
  {
    v.erase(it);
  }
}

template<typename Vector, typename Fun>
bool any_of(Vector&& v, Fun fun)
{
  return std::any_of(std::begin(v), std::end(v), fun);
}

template<typename Vector, typename Fun>
bool none_of(Vector&& v, Fun fun)
{
  return std::none_of(std::begin(v), std::end(v), fun);
}

template<typename Vector, typename Fun>
auto remove_if(Vector&& v, Fun fun)
{
  return std::remove_if(std::begin(v), std::end(v), fun);
}
}
