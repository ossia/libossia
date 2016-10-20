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
  { return {}; }

  template <typename T>
  ossia::value operator()(T&& value, const domain_base<T>& domain)
  { return domain.clamp(b, std::forward<T>(value)); }

  template <typename T>
  ossia::value operator()(const Tuple& value, const domain_base<T>& domain)
  {
    Tuple res = value;
    for(auto& val : res.value)
    {
      if(val.getType() == ossia::value_trait<T>::ossia_enum)
        val = eggs::variants::apply([&] (auto& sub_val) { return this->operator()(sub_val, domain); }, val);
    }
    return res;
  }

  template <typename T>
  ossia::value operator()(Tuple&& value, const domain_base<T>& domain)
  {
    for(auto& val : value.value)
    {
      if(val.getType() == ossia::value_trait<T>::ossia_enum)
        val = eggs::variants::apply([&] (auto& sub_val) { return this->operator()(sub_val, domain); }, val);
    }
    // TODO currently other values (strings, etc...) are ignored; what should we do here ?
    return std::move(value);
  }

  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Float>& domain)
  { return domain.clamp(b, value); }

  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Int>& domain)
  { return domain.clamp(b, value); }

  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Bool>& domain)
  { return domain.clamp(b, value); }

  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Char>& domain)
  { return domain.clamp(b, value); }
};

struct domain_min_visitor
{
  auto operator()(const domain_base<Int>& value)
  { return value.min ? value::make<Int>(value.min.get()) : ossia::value{}; }
  auto operator()(const domain_base<Float>& value)
  { return value.min ? value::make<Float>(value.min.get()) : ossia::value{}; }
  auto operator()(const domain_base<Bool>& value)
  { return value.min ? value::make<Bool>(value.min.get()) : ossia::value{}; }
  auto operator()(const domain_base<Char>& value)
  { return value.min ? value::make<Char>(value.min.get()) : ossia::value{}; }
  auto operator()(const domain_base<Tuple>& value)
  { return value.min ? value.min.get() : ossia::value{}; }

  template <typename... T>
  auto operator()(const T&...)
  {
    return ossia::value{};
  }
};

struct domain_max_visitor
{
  auto operator()(const domain_base<Int>& value)
  { return value.max ? value::make<Int>(value.max.get()) : ossia::value{}; }
  auto operator()(const domain_base<Float>& value)
  { return value.max ? value::make<Float>(value.max.get()) : ossia::value{}; }
  auto operator()(const domain_base<Bool>& value)
  { return value.max ? value::make<Bool>(value.max.get()) : ossia::value{}; }
  auto operator()(const domain_base<Char>& value)
  { return value.max ? value::make<Char>(value.max.get()) : ossia::value{}; }
  auto operator()(const domain_base<Tuple>& value)
  { return value.max ? value.max.get() : ossia::value{}; }

  template <typename... T>
  auto operator()(const T&...)
  {
    return ossia::value{};
  }
};


template<typename T>
struct domain_min_max_visitor
{
  template<typename U>
  domain_min_max<T> operator()(const domain_base<U>& value)
  { return domain_min_max<T>{value}; }
};

template<typename T>
struct domain_convert_min_max_visitor
{
  template<typename U>
  domain_min_max<T> operator()(const domain_base<U>& value)
  { return domain_min_max<T>{value}; }
};

template<typename Domain>
struct domain_set_min_visitor_impl
{
  Domain& domain;
  template<typename T>
  void operator()(const T&) { }
  void operator()() {  domain.min.reset(); }

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
  void operator()() { domain.max.reset(); }

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

struct domain_minmax_creation_visitor
{
  // Two operands overloads : used to create a domain from a min / max
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

  // Three operands overloads : used to create a domain from a min, a max, and a current value.
};

struct domain_values_creation_visitor
{
  // TODO create a domain from a set of available values.
};
}
}
