#pragma once
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
namespace net
{

OSSIA_EXPORT value clamp(const domain& dom, bounding_mode b, ossia::value&& val);
OSSIA_EXPORT value clamp(const domain& dom, bounding_mode b, const ossia::value& val);
/*
template<typename T>
OSSIA_EXPORT domain_min_max<T> min_and_max(const domain& dom);
template<>
OSSIA_EXPORT domain_min_max<Int> min_and_max(const domain& dom);
template<>
OSSIA_EXPORT domain_min_max<Float> min_and_max(const domain& dom);
template<>
OSSIA_EXPORT domain_min_max<Char> min_and_max(const domain& dom);
template<>
OSSIA_EXPORT domain_min_max<Bool> min_and_max(const domain& dom);
template<>
OSSIA_EXPORT domain_min_max<Tuple> min_and_max(const domain& dom);
*/
OSSIA_EXPORT value get_min(const domain& dom);
OSSIA_EXPORT value get_max(const domain& dom);
OSSIA_EXPORT void set_min(domain& dom, const ossia::value& val);
OSSIA_EXPORT void set_max(domain& dom, const ossia::value& val);

OSSIA_EXPORT domain make_domain(
    const ossia::value& min,
    const ossia::value& max);

OSSIA_EXPORT domain make_domain(
    const boost::container::small_vector<ossia::value, 2>& val,
    const ossia::value& cur);

OSSIA_EXPORT domain init_domain(ossia::val_type type);

OSSIA_EXPORT bool operator==(const domain_base<String>& lhs, const domain_base<String>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<Char>& lhs, const domain_base<Char>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<Bool>& lhs, const domain_base<Bool>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<Float>& lhs, const domain_base<Float>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<Int>& lhs, const domain_base<Int>& rhs);

template <typename T>
bool operator==(const domain_base<T>& lhs, const domain_base<T>& rhs)
{
  return true;
}

template <typename T>
bool operator!=(const domain_base<T>& lhs, const domain_base<T>& rhs)
{
  return !(lhs == rhs);
}

}
}
