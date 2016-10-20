#pragma once
#include <ossia/network/domain/numeric_domain.hpp>
namespace ossia
{
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
  domain_base(value_type min_v, value_type max_v) : min{min_v}, max{max_v}
  {
  }

  template<typename U>
  value clamp(bounding_mode b, U&& val) const
  {
    return numeric_clamp<domain_base<T>>{*this}(b, std::forward<U>(val));
  }
};

template <>
struct OSSIA_EXPORT domain_base<Impulse>
{
  value clamp(bounding_mode b, Impulse val) const
  {
    return val;
  }
};

template <>
struct OSSIA_EXPORT domain_base<Behavior>
{
  value clamp(bounding_mode b, const Behavior& val) const
  {
    return val;
  }
};

template <>
struct OSSIA_EXPORT domain_base<Destination>
{
  value clamp(bounding_mode b, const Destination& val) const
  {
    return val;
  }
};

template <>
struct OSSIA_EXPORT domain_base<String>
{
  domain_base() = default;

  boost::container::flat_set<std::string> values;
  value clamp(bounding_mode b, const String& val) const;
};

/** Compare all the values one by one **/
template <>
struct OSSIA_EXPORT domain_base<Tuple>
{
  boost::optional<Tuple> min;
  boost::optional<Tuple> max;
  boost::container::flat_set<Tuple> values;

  value clamp(bounding_mode b, const Tuple& val) const;
  value clamp(bounding_mode b, Tuple&& val) const;
};

/** Compare all the values one by one **/
template <std::size_t N>
struct OSSIA_EXPORT domain_base<Vec<float, N>>
{
  using value_type = float;
  boost::optional<Vec<float, N>> min;
  boost::optional<Vec<float, N>> max;
  boost::container::flat_set<Vec<float, N>> values;

  value clamp(bounding_mode b, const Vec<float, N>& val) const;
};



struct OSSIA_EXPORT generic_domain
{
  boost::optional<ossia::value> min;
  boost::optional<ossia::value> max;
  boost::container::flat_set<ossia::value> values;

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
                    domain_base<Behavior>, generic_domain>;
}
}
