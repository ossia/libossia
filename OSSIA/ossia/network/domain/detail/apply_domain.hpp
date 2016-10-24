#pragma once
#include <ossia/network/domain/domain_base.hpp>
#include <ossia/network/domain/detail/generic_domain.hpp>
#include <ossia/network/domain/detail/numeric_domain.hpp>
#include <ossia/network/domain/detail/array_domain.hpp>
#include <ossia/network/domain/detail/value_set_domain.hpp>

namespace ossia
{
namespace net
{

struct apply_domain_visitor
{
  bounding_mode b;

  // General case with incompatible values
  template <typename T, typename U>
  ossia::value operator()(const T& value, const U& bad_domain) const
  { return {}; }

  // Generic case
  template <typename T>
  ossia::value operator()(T&& value, const domain_base<ossia::value>& domain) const
  { return generic_clamp{domain}(b, std::move(value)); }

  template <typename T>
  ossia::value operator()(const T& value, const domain_base<ossia::value>& domain) const
  { return generic_clamp{domain}(b, value); }

  // Values without meaningful domains
  ossia::value operator()(Impulse value, const domain_base<Impulse>& domain) const
  { return value; }
  // Numeric values
  ossia::value operator()(Int value, const domain_base<Int>& domain) const;
  ossia::value operator()(Float value, const domain_base<Float>& domain) const;
  ossia::value operator()(Char value, const domain_base<Char>& domain) const;
  ossia::value operator()(Bool value, const domain_base<Bool>& domain) const;

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

  ossia::value operator()(const Tuple& value, const domain_base<ossia::value>& domain) const;
  ossia::value operator()(Tuple&& value, const domain_base<ossia::value>& domain) const;

  // Second case : we filter a whole tuple.
  ossia::value operator()(const Tuple& value, const domain_base<Tuple>& domain) const;
  ossia::value operator()(Tuple&& value, const domain_base<Tuple>& domain) const;

  // Vec : we can either filter each value, or filter the whole shebang
  ossia::value operator()(const Vec<float, 2>& value, const domain_base<Float>& domain) const;
  ossia::value operator()(const Vec<float, 2>& value, const domain_base<Int>& domain) const;
  ossia::value operator()(const Vec<float, 2>& value, const domain_base<Bool>& domain) const;
  ossia::value operator()(const Vec<float, 2>& value, const domain_base<Char>& domain) const;
  ossia::value operator()(const Vec<float, 2>& value, const domain_base<Vec<float, 2>>& domain) const;
  ossia::value operator()(const Vec<float, 3>& value, const domain_base<Float>& domain) const;
  ossia::value operator()(const Vec<float, 3>& value, const domain_base<Int>& domain) const;
  ossia::value operator()(const Vec<float, 3>& value, const domain_base<Bool>& domain) const;
  ossia::value operator()(const Vec<float, 3>& value, const domain_base<Char>& domain) const;
  ossia::value operator()(const Vec<float, 3>& value, const domain_base<Vec<float, 3>>& domain) const;
  ossia::value operator()(const Vec<float, 4>& value, const domain_base<Float>& domain) const;
  ossia::value operator()(const Vec<float, 4>& value, const domain_base<Int>& domain) const;
  ossia::value operator()(const Vec<float, 4>& value, const domain_base<Bool>& domain) const;
  ossia::value operator()(const Vec<float, 4>& value, const domain_base<Char>& domain) const;
  ossia::value operator()(const Vec<float, 4>& value, const domain_base<Vec<float, 4>>& domain) const;

};

template<typename Domain_T>
struct tuple_apply_domain_helper
{
  const apply_domain_visitor& vis;
  const Domain_T& dom;

  template<typename U>
  ossia::value operator()(U&& u) const
  {
    return vis(std::forward<U>(u), dom);
  }
};

template <typename T>
ossia::value apply_domain_visitor::operator()(const Tuple& value, const domain_base<T>& domain) const
{
  Tuple res = value;
  for(auto& val : res)
  {
    if(val.getType() == ossia::value_trait<T>::ossia_enum)
      val = eggs::variants::apply(tuple_apply_domain_helper<domain_base<T>>{*this, domain}, val.v);
  }
  return res;
}

template <typename T>
ossia::value apply_domain_visitor::operator()(Tuple&& value, const domain_base<T>& domain) const
{
  for(auto& val : value)
  {
    if(val.getType() == ossia::value_trait<T>::ossia_enum)
      val = eggs::variants::apply(tuple_apply_domain_helper<domain_base<T>>{*this, domain}, std::move(val.v));
  }
  // TODO currently other values (strings, etc...) are ignored; what should we do here ?
  return std::move(value);
}

}
}
