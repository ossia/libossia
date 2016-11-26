#pragma once
#include <ossia/network/domain/domain_base.hpp>
namespace ossia
{

namespace net
{
/**
 * Applying a domain value by value to arrays
 */
struct tuple_clamp
{
  const net::domain_base<std::vector<ossia::value>>& domain;

  ossia::value operator()(bounding_mode b, const std::vector<ossia::value>& val) const;
  ossia::value operator()(bounding_mode b, std::vector<ossia::value>&& val) const;
  // TODO numeric_tuple_clamp that will be used instead
  // of the loops in domain_clamp_visitor

};

template<std::size_t N>
struct vec_clamp
{
  const net::domain_base<std::array<float, N>>& domain;

  ossia::value operator()(bounding_mode b, std::array<float, N> val) const;
};

}
}
