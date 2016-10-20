#pragma once
#include <ossia/network/domain/domain_base.hpp>
#include <ossia/network/domain/detail/generic_domain.hpp>
#include <ossia/network/domain/detail/numeric_domain.hpp>
#include <ossia/network/domain/detail/array_domain.hpp>

namespace ossia
{
namespace net
{

struct domain_clamp_visitor
{
  bounding_mode b;

  // General case with incompatible values
  template <typename T, typename U>
  ossia::value operator()(T&& value, const U& bad_domain) const
  { return {}; }

  // Generic case
  template <typename T>
  ossia::value operator()(T&& value, const domain_base<ossia::value>& domain) const
  { return generic_clamp{domain}(b, std::forward<T>(value)); }

  // Values without meaningful domains
  ossia::value operator()(Impulse value, const domain_base<Impulse>& domain) const
  { return value; }
  ossia::value operator()(const Behavior& value, const domain_base<Behavior>& domain) const
  { return value; }
  ossia::value operator()(Behavior&& value, const domain_base<Behavior>& domain) const
  { return std::move(value); }
  ossia::value operator()(const Destination& value, const domain_base<Destination>& domain) const
  { return value; }
  ossia::value operator()(Destination&& value, const domain_base<Destination>& domain) const
  { return std::move(value); }

  // Numeric values
  ossia::value operator()(Int value, const domain_base<Int>& domain) const
  { return numeric_clamp<domain_base<Int>>{domain}(b, value); }
  ossia::value operator()(Float value, const domain_base<Float>& domain) const
  { return numeric_clamp<domain_base<Float>>{domain}(b, value); }
  ossia::value operator()(Char value, const domain_base<Char>& domain) const
  { return numeric_clamp<domain_base<Char>>{domain}(b, value); }
  ossia::value operator()(Bool value, const domain_base<Bool>& domain) const
  { return numeric_clamp<domain_base<Bool>>{domain}(b, value); }

  // Strings
  ossia::value operator()(const String& value, const domain_base<String>& domain) const
  { return value_set_clamp<domain_base<String>>{domain}(b, value); }
  ossia::value operator()(String&& value, const domain_base<String>& domain) const
  { return value_set_clamp<domain_base<String>>{domain}(b, std::move(value)); }


  // Tuples
  // First case : tuple with another domain : we try to filter all the values
  // of the tuple that are filterable by this domain.
  // They are defined outside the class to handle a GCC bug...
  template <typename T>
  ossia::value operator()(const Tuple& value, const domain_base<T>& domain) const;
  template <typename T>
  ossia::value operator()(Tuple&& value, const domain_base<T>& domain) const;

  ossia::value operator()(const Tuple& value, const domain_base<ossia::value>& domain) const
  {
    Tuple res = value;
    for(auto& val : res.value)
    {
      val = generic_clamp{domain}(b, val);
    }
    return res;
  }

  ossia::value operator()(Tuple&& value, const domain_base<ossia::value>& domain) const
  {
    for(auto& val : value.value)
    {
      val = generic_clamp{domain}(b, std::move(val));
    }
    // TODO currently other values (strings, etc...) are ignored; what should we do here ?
    return std::move(value);
  }

  // Second case : we filter a whole tuple.
  ossia::value operator()(const Tuple& value, const domain_base<Tuple>& domain) const
  { return tuple_clamp{domain}(b, value); }
  ossia::value operator()(Tuple&& value, const domain_base<Tuple>& domain) const
  { return tuple_clamp{domain}(b, std::move(value)); }

  // Vec : we can either filter each value, or filter the whole shebang
  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Float>& domain) const
  { return numeric_clamp<domain_base<Float>>{domain}(b, value); }

  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Int>& domain) const
  { return numeric_clamp<domain_base<Int>>{domain}(b, value); }

  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Bool>& domain) const
  { return numeric_clamp<domain_base<Bool>>{domain}(b, value); }

  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Char>& domain) const
  { return numeric_clamp<domain_base<Char>>{domain}(b, value); }

  template <int N>
  ossia::value operator()(const Vec<float, N>& value, const domain_base<Vec<float, N>>& domain) const
  { return vec_clamp<N>{domain}(b, value); }
};

template<typename Domain_T>
struct tuple_clamp_helper
{
  const domain_clamp_visitor& vis;
  const Domain_T& dom;

  template<typename U>
  ossia::value operator()(U&& u) const
  {
    return vis(std::forward<U>(u), dom);
  }
};

template <typename T>
ossia::value domain_clamp_visitor::operator()(const Tuple& value, const domain_base<T>& domain) const
{
  Tuple res = value;
  for(auto& val : res.value)
  {
    if(val.getType() == ossia::value_trait<T>::ossia_enum)
      val = eggs::variants::apply(tuple_clamp_helper<domain_base<T>>{*this, domain}, val.v);
  }
  return res;
}

template <typename T>
ossia::value domain_clamp_visitor::operator()(Tuple&& value, const domain_base<T>& domain) const
{
  for(auto& val : value.value)
  {
    if(val.getType() == ossia::value_trait<T>::ossia_enum)
      val = eggs::variants::apply(tuple_clamp_helper<domain_base<T>>{*this, domain}, std::move(val.v));
  }
  // TODO currently other values (strings, etc...) are ignored; what should we do here ?
  return std::move(value);
}


struct domain_min_visitor
{
  template<typename T, typename U = decltype(std::declval<T>().min)>
  ossia::value operator()(const domain_base<Int>& value)
  { return value.min ? value::make<Int>(value.min.get()) : ossia::value{}; }

  template <typename... T>
  auto operator()(const T&...)
  {
    return ossia::value{};
  }
};

struct domain_max_visitor
{
  template<typename T, typename U = decltype(std::declval<T>().max)>
  ossia::value operator()(const domain_base<Int>& value)
  { return value.max ? value::make<Int>(value.max.get()) : ossia::value{}; }

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


struct domain_set_min_visitor
{
  void operator()(domain_base<Int>& domain, Int incoming)
  { domain.min = incoming.value; }
  void operator()(domain_base<Float>& domain, Float incoming)
  { domain.min = incoming.value; }
  void operator()(domain_base<Char>& domain, Char incoming)
  { domain.min = incoming.value; }
  void operator()(domain_base<Bool>& domain, Bool incoming)
  { domain.min = incoming.value; }
  void operator()(domain_base<Tuple>& domain, const Tuple& incoming)
  { domain.min = incoming.value; }

  template<int N>
  void operator()(domain_base<Vec<float, N>>& domain, const Vec<float, N>& incoming)
  { domain.min = incoming; }

  template<typename T>
  void operator()(domain_base<ossia::value>& domain, const T& incoming)
  { domain.min = ossia::value{incoming}; }

  // Removal cases
  // Here we could maybe allow a cast or something like this...
  // for e.g. int -> float
  template <typename... U>
  void operator()(domain_base<Int>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<Float>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<Char>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<Bool>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<Tuple>& domain, U&&...)
  { domain.min.reset(); }
  template<int N, typename... U>
  void operator()(domain_base<Vec<float, N>>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<ossia::value>& domain, U&&...)
  { domain.min.reset(); }

  template <typename T, typename... U>
  void operator()(const domain_base<T>& domain, U&&...)
  {
  }
};

struct domain_set_max_visitor
{
  void operator()(domain_base<Int>& domain, Int incoming)
  { domain.max = incoming.value; }
  void operator()(domain_base<Float>& domain, Float incoming)
  { domain.max = incoming.value; }
  void operator()(domain_base<Char>& domain, Char incoming)
  { domain.max = incoming.value; }
  void operator()(domain_base<Bool>& domain, Bool incoming)
  { domain.max = incoming.value; }
  void operator()(domain_base<Tuple>& domain, const Tuple& incoming)
  { domain.max = incoming.value; }

  template<int N>
  void operator()(domain_base<Vec<float, N>>& domain, const Vec<float, N>& incoming)
  { domain.max = incoming; }

  template<typename T>
  void operator()(domain_base<ossia::value>& domain, const T& incoming)
  { domain.max = ossia::value{incoming}; }

  // Removal cases
  template <typename... U>
  void operator()(domain_base<Int>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<Float>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<Char>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<Bool>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<Tuple>& domain, U&&...)
  { domain.max.reset(); }
  template<int N, typename... U>
  void operator()(domain_base<Vec<float, N>>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<ossia::value>& domain, U&&...)
  { domain.max.reset(); }

  template <typename T, typename... U>
  void operator()(const domain_base<T>& domain, U&&...)
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
  const boost::container::small_vector<ossia::value, 2>& values;
  // TODO create a domain from a set of available values.
  template <typename T>
  domain operator()(const T& min)
  {
    // Cases where there is no possible domain
    return domain{};
  }

};
}
}
