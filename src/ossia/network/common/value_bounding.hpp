#pragma once
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/value/value.hpp>

namespace ossia
{

template <typename Value_T>
inline ossia::value
bound_value(const ossia::domain& dom, Value_T&& base_val, ossia::bounding_mode mode)
{
  if(dom)
  {
    auto res = ossia::apply_domain(dom, mode, std::forward<Value_T>(base_val));
    if(res.valid())
      return res;
    else
      return {};
  }
  else
  {
    return std::forward<Value_T>(base_val);
  }
}

template <typename Addr_T>
inline ossia::value bound_value(const Addr_T& addr, const ossia::value& v)
{
  auto val = bound_value(addr.get_domain(), v, addr.get_bounding());
  auto filtered = addr.filter_value(val);
  if(!filtered)
    return val;
  return {};
}

template <typename Addr_T>
inline ossia::value bound_value(const Addr_T& addr, ossia::value&& v)
{
  auto val = bound_value(addr.get_domain(), std::move(v), addr.get_bounding());
  auto filtered = addr.filter_value(val);
  if(!filtered)
    return val;
  return {};
}

}
