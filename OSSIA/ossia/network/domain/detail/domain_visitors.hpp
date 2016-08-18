#pragma once
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
namespace net
{
struct domain_clamp_visitor
{
  bounding_mode b;

  template <typename T, typename U>
  ossia::value operator()(T&& value, const U& bad_domain)
  {
    return std::forward<T>(value);
  }

  template <typename T>
  ossia::value operator()(T&& value, const domain_base<T>& domain)
  {
    return domain.clamp(b, std::forward<T>(value));
  }
};

struct domain_min_visitor
{
  auto operator()(const domain_base<Int>& value)
  { return value.min ? value::make<Int>(*value.min) : ossia::value{}; }
  auto operator()(const domain_base<Float>& value)
  { return value.min ? value::make<Float>(*value.min) : ossia::value{}; }
  auto operator()(const domain_base<Bool>& value)
  { return value.min ? value::make<Bool>(*value.min) : ossia::value{}; }
  auto operator()(const domain_base<Char>& value)
  { return value.min ? value::make<Char>(*value.min) : ossia::value{}; }
  auto operator()(const domain_base<Tuple>& value)
  { return value.min ? *value.min : ossia::value{}; }

  template <typename... T>
  auto operator()(const T&...)
  {
    return ossia::value{};
  }
};

struct domain_max_visitor
{
  auto operator()(const domain_base<Int>& value)
  { return value.max ? value::make<Int>(*value.max) : ossia::value{}; }
  auto operator()(const domain_base<Float>& value)
  { return value.max ? value::make<Float>(*value.max) : ossia::value{}; }
  auto operator()(const domain_base<Bool>& value)
  { return value.max ? value::make<Bool>(*value.max) : ossia::value{}; }
  auto operator()(const domain_base<Char>& value)
  { return value.max ? value::make<Char>(*value.max) : ossia::value{}; }
  auto operator()(const domain_base<Tuple>& value)
  { return value.max ? *value.max : ossia::value{}; }

  template <typename... T>
  auto operator()(const T&...)
  {
    return ossia::value{};
  }
};

template<typename Domain>
struct domain_set_min_visitor_impl
{
  Domain& domain;
  template<typename T>
  void operator()(const T&) { }
  void operator()() { }

  void operator()(Int v) { domain.min = v.value; }
  void operator()(Float v) { domain.min = v.value; }
  void operator()(Char v) { domain.min = v.value; }
  void operator()(Bool v) { domain.min = v.value; }
};

template<typename Domain>
struct domain_set_max_visitor_impl
{
  Domain& domain;
  template<typename T>
  void operator()(const T&) { }
  void operator()() { }

  void operator()(Int v) { domain.max = v.value; }
  void operator()(Float v) { domain.max = v.value; }
  void operator()(Char v) { domain.max = v.value; }
  void operator()(Bool v) { domain.max = v.value; }
};

struct domain_set_min_visitor
{
  const ossia::value& val;

  void operator()(domain_base<Int>& value)
  { val.apply(domain_set_min_visitor_impl<domain_base<Int>>{value}); }
  void operator()(domain_base<Float>& value)
  { val.apply(domain_set_min_visitor_impl<domain_base<Float>>{value}); }
  void operator()(domain_base<Bool>& value)
  { val.apply(domain_set_min_visitor_impl<domain_base<Bool>>{value}); }
  void operator()(domain_base<Char>& value)
  { val.apply(domain_set_min_visitor_impl<domain_base<Char>>{value}); }
  void operator()(domain_base<Tuple>& value)
  { value.min = val; }

  template <typename... T>
  void operator()(const T&...)
  {
  }
};

struct domain_set_max_visitor
{
  const ossia::value& val;

  void operator()(domain_base<Int>& value)
  { val.apply(domain_set_max_visitor_impl<domain_base<Int>>{value}); }
  void operator()(domain_base<Float>& value)
  { val.apply(domain_set_max_visitor_impl<domain_base<Float>>{value}); }
  void operator()(domain_base<Bool>& value)
  { val.apply(domain_set_max_visitor_impl<domain_base<Bool>>{value}); }
  void operator()(domain_base<Char>& value)
  { val.apply(domain_set_max_visitor_impl<domain_base<Char>>{value}); }
  void operator()(domain_base<Tuple>& value)
  { value.max = val; }

  template <typename... T>
  void operator()(const T&...)
  {
  }
};

struct domain_creation_visitor
{
  domain operator()(const Int& min, const Int& max)
  {
    return domain_base<Int>{min.value, max.value};
  }
  domain operator()(const Float& min, const Float& max)
  {
    return domain_base<Float>{min.value, max.value};
  }
  domain operator()(const Bool& min, const Bool& max)
  {
    return domain_base<Bool>{min.value, max.value};
  }
  domain operator()(const Char& min, const Char& max)
  {
    return domain_base<Char>{min.value, max.value};
  }

  template <typename T>
  domain operator()(const T& min, const T& max)
  {
    // Cases where the domain has no min-max
    return domain_base<T>();
  }

  template <typename T, typename U>
  domain operator()(const T& min, const U& max)
  {
    // Cases where there is no possible domain
    return domain{};
  }
};
}
}
