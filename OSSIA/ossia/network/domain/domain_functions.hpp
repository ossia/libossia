#pragma once
#include <ossia_export.h>
#include <chobo/small_vector.hpp>
#include <ossia/editor/value/value_base.hpp>
namespace ossia
{
class value;
namespace net
{
template<typename T>
struct domain_base;
struct domain;

OSSIA_EXPORT value apply_domain(
    const domain& dom,
    bounding_mode b,
    const ossia::value& val);
OSSIA_EXPORT value apply_domain(
    const domain& dom,
    bounding_mode b,
    ossia::value&& val);

OSSIA_EXPORT value get_min(const domain& dom);
OSSIA_EXPORT value get_max(const domain& dom);
OSSIA_EXPORT void set_min(domain& dom, const ossia::value& val);
OSSIA_EXPORT void set_max(domain& dom, const ossia::value& val);

OSSIA_EXPORT void set_values(domain& dom, const std::vector<ossia::value>& val);

OSSIA_EXPORT domain make_domain(
    const ossia::value& min,
    const ossia::value& max);

OSSIA_EXPORT domain make_domain(
    const chobo::small_vector<ossia::value, 2>& val,
    const ossia::value& cur);

OSSIA_EXPORT domain make_domain(
    const ossia::value& min,
    const ossia::value& max,
    const std::vector<ossia::value>& vals);

OSSIA_EXPORT domain init_domain(
    ossia::val_type type);

OSSIA_EXPORT bool operator==(const domain_base<impulse>& lhs, const domain_base<impulse>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<std::string>& lhs, const domain_base<std::string>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<char>& lhs, const domain_base<char>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<bool>& lhs, const domain_base<bool>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<float>& lhs, const domain_base<float>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<int32_t>& lhs, const domain_base<int32_t>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<std::vector<ossia::value>>& lhs, const domain_base<std::vector<ossia::value>>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<vec2f>& lhs, const domain_base<vec2f>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<vec3f>& lhs, const domain_base<vec3f>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<vec4f>& lhs, const domain_base<vec4f>& rhs);
OSSIA_EXPORT bool operator==(const domain_base<ossia::value>& lhs, const domain_base<ossia::value>& rhs);

template <typename T>
bool operator!=(const domain_base<T>& lhs, const domain_base<T>& rhs)
{
  return !(lhs == rhs);
}

}
}

std::ostream& operator<<(std::ostream&, const ossia::net::domain&);
