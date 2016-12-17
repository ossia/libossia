#pragma once
#include <ossia/editor/value/value_traits.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <boost/container/flat_set.hpp>
#include <ossia/detail/optional.hpp>
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
  ossia::optional<value_type> min;
  ossia::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base() noexcept { }
  domain_base(const domain_base& other) noexcept:
    min{other.min},
    max{other.max},
    values{other.values}
  {

  }

  domain_base(domain_base&& other) noexcept :
    min{std::move(other.min)},
    max{std::move(other.max)},
    values{std::move(other.values)}
  {

  }

  domain_base& operator=(const domain_base& other)
  {
    min = other.min;
    max = other.max;
    values = other.values;
    return *this;
  }

  domain_base& operator=(domain_base&& other) noexcept
  {
    min = std::move(other.min);
    max = std::move(other.max);
    values= std::move(other.values);
    return *this;
  }

  domain_base(value_type v1, value_type v2): min{v1}, max{v2} { }
};

template <>
struct OSSIA_EXPORT domain_base<Impulse>
{
};

template <>
struct OSSIA_EXPORT domain_base<std::string>
{
  boost::container::flat_set<std::string> values;
};

template <>
struct OSSIA_EXPORT domain_base<std::vector<ossia::value>>
{
  using value_type = std::vector<ossia::value>;
  ossia::optional<value_type> min;
  ossia::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base() noexcept { }
  domain_base(const domain_base<value_type>& other) noexcept :
    min{std::move(other.min)},
    max{std::move(other.max)},
    values{std::move(other.values)}
  {

  }

  domain_base(domain_base<value_type>&& other) noexcept  :
    min{std::move(other.min)},
    max{std::move(other.max)},
    values{std::move(other.values)}
  {

  }

  domain_base& operator=(const domain_base& other)
  {
    min = other.min;
    max = other.max;
    values = other.values;
    return *this;
  }

  domain_base& operator=(domain_base&& other) noexcept
  {
    min = std::move(other.min);
    max = std::move(other.max);
    values= std::move(other.values);
    return *this;
  }

  domain_base(const value_type& v1, const value_type& v2): min{v1}, max{v2} { }
  domain_base(value_type&& v1, value_type&& v2): min{std::move(v1)}, max{std::move(v2)} { }
};

template <std::size_t N>
struct OSSIA_EXPORT domain_base<std::array<float, N>>
{
  using value_type = std::array<float, N>;
  ossia::optional<std::array<float, N>> min;
  ossia::optional<std::array<float, N>> max;
  boost::container::flat_set<std::array<float, N>> values;

  domain_base() noexcept { }
  domain_base(const domain_base<value_type>& other) noexcept :
    min{std::move(other.min)},
    max{std::move(other.max)},
    values{std::move(other.values)}
  {

  }

  domain_base(domain_base<value_type>&& other) noexcept  :
    min{std::move(other.min)},
    max{std::move(other.max)},
    values{std::move(other.values)}
  {

  }

  domain_base& operator=(const domain_base& other)
  {
    min = other.min;
    max = other.max;
    values = other.values;
    return *this;
  }

  domain_base& operator=(domain_base&& other) noexcept
  {
    min = std::move(other.min);
    max = std::move(other.max);
    values= std::move(other.values);
    return *this;
  }

  domain_base<value_type>(const value_type& v1, const value_type& v2): min{v1}, max{v2} { }
};

template <>
struct OSSIA_EXPORT domain_base<ossia::value>
{
  using value_type = ossia::value;
  ossia::optional<value_type> min;
  ossia::optional<value_type> max;
  boost::container::flat_set<value_type> values;


  domain_base() noexcept { }
  domain_base(const domain_base<value_type>& other) noexcept :
    min{std::move(other.min)},
    max{std::move(other.max)},
    values{std::move(other.values)}
  {

  }

  domain_base(domain_base<value_type>&& other) noexcept  :
    min{std::move(other.min)},
    max{std::move(other.max)},
    values{std::move(other.values)}
  {

  }

  domain_base& operator=(const domain_base& other)
  {
    min = other.min;
    max = other.max;
    values = other.values;
    return *this;
  }

  domain_base& operator=(domain_base&& other) noexcept
  {
    min = std::move(other.min);
    max = std::move(other.max);
    values= std::move(other.values);
    return *this;
  }

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
using domain_base_variant = eggs::variant<domain_base<Impulse>, domain_base<bool>, domain_base<int32_t>,
                    domain_base<float>, domain_base<char>, domain_base<std::string>,
                    domain_base<std::vector<ossia::value>>, domain_base<Vec2f>, domain_base<Vec3f>,
                    domain_base<Vec4f>, domain_base<ossia::value>>;

struct OSSIA_EXPORT domain final : public domain_base_variant
{
  OSSIA_DECL_RELAXED_CONSTEXPR domain() noexcept { }

  template<typename T>
  domain(const ossia::net::domain_base<T>& arg): domain_base_variant(arg) { }
  domain(const domain& d) : domain_base_variant{ (const domain_base_variant&)d } { }
  domain(domain&& d) : domain_base_variant{ std::move((domain_base_variant&)d) } { }
  domain& operator=(const domain& d) { ((domain_base_variant&)(*this)) = (const domain_base_variant&)d; return *this; }
  domain& operator=(domain&& d) { ((domain_base_variant&)(*this)) = std::move((domain_base_variant&)d); return *this; }

  value get_min() const;
  value get_max() const;

  template<typename T>
  OSSIA_INLINE T get_min() const { return get_min().get<T>(); }
  template<typename T>
  OSSIA_INLINE T get_max() const { return get_max().get<T>(); }

  template<typename T>
  ossia::optional<T> maybe_min() const
  {
    auto v = get_min();
    auto u = v.target<T>();
    if(u)
      return *u;
    else
      return {};
  }

  template<typename T>
  ossia::optional<T> maybe_max() const
  {
    auto v = get_max();
    auto u = v.target<T>();
    if(u)
      return *u;
    else
      return {};
  }

  template<typename T>
  OSSIA_INLINE T convert_min() const { return ossia::convert<T>(get_min()); }
  template<typename T>
  OSSIA_INLINE T convert_max() const { return ossia::convert<T>(get_max()); }

  void set_min(const ossia::value& val);
  void set_max(const ossia::value& val);

  value apply(
      bounding_mode b,
      const ossia::value& val) const;
  value apply(
      bounding_mode b,
      ossia::value&& val) const;

  std::string to_pretty_string() const;
};
}
}
