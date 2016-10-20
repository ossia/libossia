#pragma once
#include <ossia/editor/value/value_traits.hpp>
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
  using ossia_type = T;
  using value_type = typename value_trait<T>::value_type;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base() = default;
  domain_base(const domain_base&) = default;
  domain_base(domain_base&&) = default;
  domain_base& operator=(const domain_base&) = default;
  domain_base& operator=(domain_base&&) = default;

  domain_base(ossia_type v1, ossia_type v2): min{v1.value}, max{v2.value} { }
  domain_base(value_type v1, value_type v2): min{v1}, max{v2} { }
};

template <>
struct OSSIA_EXPORT domain_base<Impulse>
{
};

template <>
struct OSSIA_EXPORT domain_base<Behavior>
{
};

template <>
struct OSSIA_EXPORT domain_base<Destination>
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
  using ossia_type = Tuple;
  using value_type = Tuple;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base<ossia_type>() = default;
  domain_base<ossia_type>(const domain_base<ossia_type>&) = default;
  domain_base<ossia_type>(domain_base<ossia_type>&&) = default;
  domain_base<ossia_type>& operator=(const domain_base<ossia_type>&) = default;
  domain_base<ossia_type>& operator=(domain_base<ossia_type>&&) = default;

  domain_base<ossia_type>(const ossia_type& v1, const ossia_type& v2): min{v1}, max{v2} { }
  domain_base<ossia_type>(ossia_type&& v1, ossia_type&& v2): min{std::move(v1)}, max{std::move(v2)} { }
};

template <int N>
struct OSSIA_EXPORT domain_base<Vec<float, N>>
{
  using ossia_type = Vec<float, N>;
  using value_type = Vec<float, N>;
  boost::optional<Vec<float, N>> min;
  boost::optional<Vec<float, N>> max;
  boost::container::flat_set<Vec<float, N>> values;

  domain_base<ossia_type>() = default;
  domain_base<ossia_type>(const domain_base<ossia_type>&) = default;
  domain_base<ossia_type>(domain_base<ossia_type>&&) = default;
  domain_base<ossia_type>& operator=(const domain_base<ossia_type>&) = default;
  domain_base<ossia_type>& operator=(domain_base<ossia_type>&&) = default;

  domain_base<ossia_type>(const ossia_type& v1, const ossia_type& v2): min{v1.value}, max{v2.value} { }
};

template <>
struct OSSIA_EXPORT domain_base<ossia::value>
{
  using ossia_type = ossia::value;
  using value_type = ossia::value;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base<ossia_type>() = default;
  domain_base<ossia_type>(const domain_base<ossia_type>&) = default;
  domain_base<ossia_type>(domain_base<ossia_type>&&) = default;
  domain_base<ossia_type>& operator=(const domain_base<ossia_type>&) = default;
  domain_base<ossia_type>& operator=(domain_base<ossia_type>&&) = default;

  domain_base<ossia_type>(const ossia_type& v1, const ossia_type& v2): min{v1}, max{v2} { }
  domain_base<ossia_type>(ossia_type&& v1, ossia_type&& v2): min{std::move(v1)}, max{std::move(v2)} { }
};

template<typename T>
struct domain_min_max
{
  domain_min_max(const domain_base<T>& dom):
    min{dom.min},
    max{dom.max}
  {

  }

  decltype(domain_base<T>::min) min;
  decltype(domain_base<T>::max) max;
};


template<>
struct OSSIA_EXPORT domain_min_max<Impulse>
{
  domain_min_max(const domain_base<Impulse>& dom) { }
};

template<>
struct OSSIA_EXPORT domain_min_max<String>
{
  domain_min_max(const domain_base<String>& dom) { }
};

template<>
struct OSSIA_EXPORT domain_min_max<Destination>
{
  domain_min_max(const domain_base<Destination>& dom) { }
};

template<>
struct OSSIA_EXPORT domain_min_max<Behavior>
{
  domain_min_max(const domain_base<Behavior>& dom) { }
};
/**
 * \typedef domain A domain of values
 *
 * The practical domain may be different according to the value type.
 * For instance, \ref domain_base<Impulse> has no min nor max.
 *
 * Domains for Int, Bool, Float, Char have a min, a max and a set of values.
 * Domain for String has a set of values.
 * Other domains are not implemented yet.
 *
 * A domain_base should implement the following method :
 * \code
 * ossia::value clamp(bounding_mode, const T&) const
 * \endcode
 *
 * It is used to restrict a value to the domain if available.
 */
using domain
    = eggs::variant<domain_base<Impulse>, domain_base<Bool>, domain_base<Int>,
                    domain_base<Float>, domain_base<Char>, domain_base<String>,
                    domain_base<Tuple>, domain_base<Vec2f>, domain_base<Vec3f>,
                    domain_base<Vec4f>, domain_base<Destination>,
                    domain_base<Behavior>, domain_base<ossia::value>>;
}
}
