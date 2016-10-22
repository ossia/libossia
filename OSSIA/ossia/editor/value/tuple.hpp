#pragma once
#include <initializer_list>
#include <ossia_export.h>
#include <vector>

namespace ossia
{
class value;
using Tuple = std::vector<ossia::value>;
///**
// * @class Tuple tuple.hpp ossia/editor/value/value.hpp
// *
// * A tuple is a vector of \ref ossia::value.
// * Using a tuple incurs a dynamic allocation.
// */
//struct OSSIA_EXPORT Tuple
//{
//  using value_type = ossia::value;
//  using impl_type = std::vector<value_type>;
//  impl_type value;

//  operator const impl_type&() const { return value; }
//  operator impl_type&() { return value; }

//  auto& operator[](std::size_t i) { return value[i]; }
//  auto& operator[](std::size_t i) const { return value[i]; }

//  std::size_t size() const { return value.size(); }
//  bool empty() const { return value.empty(); }
//  void resize(std::size_t n) { return value.resize(n); }
//  void reserve(std::size_t n) { return value.reserve(n); }
//  template<typename Arg>
//  void push_back(Arg&& arg) { return value.push_back(std::forward<Arg>(arg)); }
//  auto begin() { return value.begin(); }
//  auto end() { return value.end(); }
//  auto begin() const { return value.begin(); }
//  auto end() const { return value.end(); }
//  auto cbegin() const { return value.cbegin(); }
//  auto cend() const { return value.cend(); }
//  Tuple();
//  Tuple(const Tuple&) = default;
//  Tuple(Tuple&&) = default;
//  Tuple& operator=(const Tuple&) = default;
//  Tuple& operator=(Tuple&&) = default;

//  explicit Tuple(const ossia::value& v);

//  Tuple(std::initializer_list<ossia::value> v);

//  Tuple(const impl_type& v);
//  Tuple(impl_type&& v);

//  bool operator==(const ossia::value&) const;
//  bool operator!=(const ossia::value&) const;
//  bool operator>(const ossia::value&) const;
//  bool operator>=(const ossia::value&) const;
//  bool operator<(const ossia::value&) const;
//  bool operator<=(const ossia::value&) const;
//};
}
