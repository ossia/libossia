#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/small_vector.hpp>
#include <span>
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/value/value_base.hpp>

#include <optional>
#include <utility>
#include <vector>
namespace ossia
{
class value;
template <typename T>
struct domain_base;
struct domain;
struct vector_domain;
template <std::size_t N>
struct vecf_domain;

OSSIA_EXPORT value
apply_domain(const domain& dom, bounding_mode b, const ossia::value& val);
OSSIA_EXPORT value apply_domain(const domain& dom, bounding_mode b, ossia::value&& val);

OSSIA_EXPORT value get_min(const domain& dom);
OSSIA_EXPORT value get_max(const domain& dom);
OSSIA_EXPORT std::pair<std::optional<float>, std::optional<float>>
get_float_minmax(const domain& dom);

OSSIA_EXPORT void set_min(domain& dom, const ossia::value& val);
OSSIA_EXPORT void set_max(domain& dom, const ossia::value& val);

OSSIA_EXPORT void set_values(domain& dom, const std::vector<ossia::value>& val);
OSSIA_EXPORT std::vector<ossia::value> get_values(const ossia::domain& dom);

// TODO we should find a way to prevent invalid domains here, e.g. min=int,
// max=float
OSSIA_EXPORT
domain make_domain(const ossia::value& lhs, const ossia::value& rhs);

OSSIA_EXPORT domain make_domain(std::vector<std::string> s);
OSSIA_EXPORT domain make_domain(std::span<const char*>);

OSSIA_EXPORT domain make_domain_from_osc(
    const ossia::small_vector<ossia::value, 2>& val, const ossia::value& cur);
OSSIA_EXPORT domain make_domain(
    const ossia::value& min, const ossia::value& max,
    const std::vector<ossia::value>& vals);
OSSIA_EXPORT domain make_domain(const std::vector<ossia::value>& vals);

OSSIA_EXPORT
ossia::domain make_domain_from_minmax(
    const std::vector<ossia::value>& min, const std::vector<ossia::value>& max,
    ossia::val_type v);

OSSIA_EXPORT domain init_domain(ossia::val_type type);

template <typename T>
bool operator!=(const domain_base<T>& lhs, const domain_base<T>& rhs)
{
  return !(lhs == rhs);
}
}

OSSIA_EXPORT
std::ostream& operator<<(std::ostream&, const ossia::domain&);
