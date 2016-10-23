#pragma once
#include <ossia/editor/value/value_traits.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/optional.hpp>
#include <type_traits>

namespace ossia
{
OSSIA_EXPORT ossia::value clamp(const ossia::value& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value wrap(const ossia::value& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value fold(const ossia::value& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value clamp_min(const ossia::value& val, const ossia::value& min);
OSSIA_EXPORT ossia::value clamp_max(const ossia::value& val, const ossia::value& max);

OSSIA_EXPORT ossia::value clamp(ossia::value&& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value wrap(ossia::value&& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value fold(ossia::value&& val, const ossia::value& min, const ossia::value& max);
OSSIA_EXPORT ossia::value clamp_min(ossia::value&& val, const ossia::value& min);
OSSIA_EXPORT ossia::value clamp_max(ossia::value&& val, const ossia::value& max);

namespace net
{
template <typename T>
struct OSSIA_EXPORT domain_base
{
  using value_type = typename value_trait<T>::value_type;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base() noexcept = default;
  domain_base(const domain_base&) = default;
  domain_base(domain_base&&) noexcept = default;
  domain_base& operator=(const domain_base&) = default;
  domain_base& operator=(domain_base&&) noexcept = default;

  domain_base(value_type v1, value_type v2): min{v1}, max{v2} { }
};

template <>
struct OSSIA_EXPORT domain_base<Impulse>
{
};

template <>
struct OSSIA_EXPORT domain_base<String>
{
  boost::container::flat_set<std::string> values;
};

template <>
struct OSSIA_EXPORT domain_base<Tuple>
{
  using value_type = Tuple;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base<value_type>() noexcept = default;
  domain_base<value_type>(const domain_base<value_type>&) = default;
  domain_base<value_type>(domain_base<value_type>&&) noexcept = default;
  domain_base<value_type>& operator=(const domain_base<value_type>&) = default;
  domain_base<value_type>& operator=(domain_base<value_type>&&) noexcept = default;

  domain_base<value_type>(const value_type& v1, const value_type& v2): min{v1}, max{v2} { }
  domain_base<value_type>(value_type&& v1, value_type&& v2): min{std::move(v1)}, max{std::move(v2)} { }
};

template <std::size_t N>
struct OSSIA_EXPORT domain_base<Vec<float, N>>
{
  using value_type = Vec<float, N>;
  boost::optional<Vec<float, N>> min;
  boost::optional<Vec<float, N>> max;
  boost::container::flat_set<Vec<float, N>> values;

  domain_base<value_type>() noexcept = default;
  domain_base<value_type>(const domain_base<value_type>&) = default;
  domain_base<value_type>(domain_base<value_type>&&) noexcept = default;
  domain_base<value_type>& operator=(const domain_base<value_type>&) = default;
  domain_base<value_type>& operator=(domain_base<value_type>&&) noexcept = default;

  domain_base<value_type>(const value_type& v1, const value_type& v2): min{v1}, max{v2} { }
};

template <>
struct OSSIA_EXPORT domain_base<ossia::value>
{
  using value_type = ossia::value;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base<value_type>() noexcept = default;
  domain_base<value_type>(const domain_base<value_type>&) = default;
  domain_base<value_type>(domain_base<value_type>&&) noexcept = default;
  domain_base<value_type>& operator=(const domain_base<value_type>&) = default;
  domain_base<value_type>& operator=(domain_base<value_type>&&) noexcept = default;

  domain_base<value_type>(const value_type& v1, const value_type& v2): min{v1}, max{v2} { }
  domain_base<value_type>(value_type&& v1, value_type&& v2): min{std::move(v1)}, max{std::move(v2)} { }
};

/**
 * \typedef domain A domain of values
 *
 * The practical domain may be different according to the value type.
 * For instance, \ref domain_base<Impulse> has no min nor max.
 *
 * Domains for most types have a min, a max and a set of values.
 * Domain for String has a set of values.
 *
 * It is used to restrict a value to the domain if available.
 */
using domain_base_variant = eggs::variant<domain_base<Impulse>, domain_base<Bool>, domain_base<Int>,
                    domain_base<Float>, domain_base<Char>, domain_base<String>,
                    domain_base<Tuple>, domain_base<Vec2f>, domain_base<Vec3f>,
                    domain_base<Vec4f>, domain_base<ossia::value>>;

struct OSSIA_EXPORT domain final : public domain_base_variant
{
  using domain_base_variant::domain_base_variant;
  OSSIA_DECL_RELAXED_CONSTEXPR domain() noexcept = default;
  domain(const domain&) = default;
  domain(domain&&) noexcept = default;
  domain& operator=(const domain&) = default;
  domain& operator=(domain&&) noexcept = default;
  value get_min() const;
  value get_max() const;

  template<typename T>
  T get_min() const { return get_min().get<T>(); }
  template<typename T>
  T get_max() const { return get_max().get<T>(); }

  template<typename T>
  boost::optional<T> maybe_min() const
  {
    auto v = get_min();
    auto u = v.target<T>();
    if(u)
      return *u;
    else
      return {};
  }

  template<typename T>
  boost::optional<T> maybe_max() const
  {
    auto v = get_max();
    auto u = v.target<T>();
    if(u)
      return *u;
    else
      return {};
  }

  template<typename T>
  T convert_min() const { return ossia::convert<T>(get_min()); }
  template<typename T>
  T convert_max() const { return ossia::convert<T>(get_max()); }

  void set_min(const ossia::value& val);
  void set_max(const ossia::value& val);

  value apply(
      bounding_mode b,
      const ossia::value& val) const;
  value apply(
      bounding_mode b,
      ossia::value&& val) const;
};
}
}
