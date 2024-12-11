#pragma once
#include <ossia/detail/flat_set.hpp>
#include <ossia/detail/small_vector.hpp>
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
namespace detail
{
/// Min ///

template <typename T>
struct domain_min_visitor_helper
{
  OSSIA_INLINE ossia::value operator()(const T& value) const
  {
    if(value.min)
      return value::make<typename T::value_type>(*value.min);
    else
      return ossia::value{};
  }
};

template <>
struct domain_min_visitor_helper<domain_base<bool>>
{
  OSSIA_INLINE ossia::value operator()(const domain_base<bool>& value) const
  {
    return value.min;
  }
};

template <>
struct domain_min_visitor_helper<domain_base<impulse>>
{
  OSSIA_INLINE ossia::value operator()(const domain_base<impulse>& value) const
  {
    return impulse{};
  }
};

template <>
struct domain_min_visitor_helper<domain_base<std::string>>
{
  OSSIA_INLINE ossia::value operator()(const domain_base<std::string>& value) const
  {
    return {};
  }
};

template <>
struct domain_min_visitor_helper<domain_base<ossia::value>>
{
  OSSIA_INLINE ossia::value operator()(const domain_base<ossia::value>& value) const
  {
    // TODO for this case, it would maybe be better to
    // use the empty state of value instead of a boost::optional ?
    if(value.min)
      return *value.min;
    else
      return ossia::value{};
  }
};

template <>
struct domain_min_visitor_helper<vector_domain>
{
  OSSIA_INLINE ossia::value operator()(const vector_domain& value) const
  {
    return value.min;
  }
};

template <std::size_t N>
struct domain_min_visitor_helper<vecf_domain<N>>
{
  OSSIA_INLINE ossia::value operator()(const vecf_domain<N>& value) const
  {
    // TODO for this case, it would maybe be better to
    // use the empty state of value instead of a boost::optional ?
    std::array<float, N> arr;
#if !defined(OSSIA_FREESTANDING)
    for(auto& val : value.min)
      if(!val)
        return ossia::value{};

    for(std::size_t i = 0; i < N; i++)
    {
      arr[i] = *value.min[i];
    }
#endif
    return arr;
  }
};

/// Max ///
template <typename T>
struct domain_max_visitor_helper
{
  OSSIA_INLINE ossia::value operator()(const T& value) const
  {
    if(value.max)
      return value::make<typename T::value_type>(*value.max);
    else
      return ossia::value{};
  }
};

template <>
struct domain_max_visitor_helper<domain_base<bool>>
{
  OSSIA_INLINE ossia::value operator()(const domain_base<bool>& value) const
  {
    return value.max;
  }
};

template <>
struct domain_max_visitor_helper<domain_base<impulse>>
{
  OSSIA_INLINE ossia::value operator()(const domain_base<impulse>& value) const
  {
    return impulse{};
  }
};

template <>
struct domain_max_visitor_helper<domain_base<std::string>>
{
  OSSIA_INLINE ossia::value operator()(const domain_base<std::string>& value) const
  {
    return {};
  }
};

template <>
struct domain_max_visitor_helper<domain_base<ossia::value>>
{
  OSSIA_INLINE ossia::value operator()(const domain_base<ossia::value>& value) const
  {
    // TODO for this case, it would maybe be better to
    // use the empty state of value instead of a boost::optional ?
    if(value.max)
      return *value.max;
    else
      return ossia::value{};
  }
};

template <>
struct domain_max_visitor_helper<vector_domain>
{
  OSSIA_INLINE ossia::value operator()(const vector_domain& value) const
  {
    return value.max;
  }
};

template <std::size_t N>
struct domain_max_visitor_helper<vecf_domain<N>>
{
  OSSIA_INLINE ossia::value operator()(const vecf_domain<N>& value) const
  {
    std::array<float, N> arr;
#if !defined(OSSIA_FREESTANDING)
    for(auto& val : value.max)
      if(!val)
        return ossia::value{};

    for(std::size_t i = 0; i < N; i++)
    {
      arr[i] = *value.max[i];
    }
#endif
    return arr;
  }
};

//// Float min-max
using float_minmax = std::pair<std::optional<float>, std::optional<float>>;
template <typename T>
struct domain_float_minmax_visitor_helper
{
  OSSIA_INLINE float_minmax operator()(const T& value) const
  {
    float_minmax ret;
    if(value.min)
      ret.first = *value.min;
    if(value.max)
      ret.second = *value.max;
    return ret;
  }
};

template <>
struct domain_float_minmax_visitor_helper<domain_base<bool>>
{
  OSSIA_INLINE float_minmax operator()(const domain_base<bool>& value) const
  {
    return std::make_pair(0.f, 1.f);
  }
};

template <>
struct domain_float_minmax_visitor_helper<domain_base<impulse>>
{
  OSSIA_INLINE float_minmax operator()(const domain_base<impulse>& value) const
  {
    return {};
  }
};

template <>
struct domain_float_minmax_visitor_helper<domain_base<std::string>>
{
  OSSIA_INLINE float_minmax operator()(const domain_base<std::string>& value) const
  {
    return {};
  }
};

template <>
struct domain_float_minmax_visitor_helper<domain_base<ossia::value>>
{
  OSSIA_INLINE float_minmax operator()(const domain_base<ossia::value>& value) const
  {
    // TODO for this case, it would maybe be better to
    // use the empty state of value instead of a boost::optional ?

    float_minmax ret;
    if(value.min)
      ret.first = ossia::convert<float>(*value.min);
    if(value.max)
      ret.second = ossia::convert<float>(*value.max);
    return ret;
  }
};

template <>
struct domain_float_minmax_visitor_helper<vector_domain>
{
  OSSIA_INLINE float_minmax operator()(const vector_domain& value) const { return {}; }
};

template <std::size_t N>
struct domain_float_minmax_visitor_helper<vecf_domain<N>>
{
  OSSIA_INLINE float_minmax operator()(const vecf_domain<N>& value) const { return {}; }
};

}

struct domain_min_visitor
{
  template <typename T>
  OSSIA_INLINE ossia::value operator()(const domain_base<T>& value) const
  {
    return detail::domain_min_visitor_helper<domain_base<T>>{}(value);
  }

  OSSIA_INLINE ossia::value operator()(const vector_domain& value) const
  {
    return detail::domain_min_visitor_helper<vector_domain>{}(value);
  }

  template <std::size_t N>
  OSSIA_INLINE ossia::value operator()(const vecf_domain<N>& value) const
  {
    return detail::domain_min_visitor_helper<vecf_domain<N>>{}(value);
  }

  template <typename... T>
  OSSIA_INLINE ossia::value operator()(const T&...) const
  {
    return ossia::value{};
  }
};

struct domain_max_visitor
{
  template <typename T>
  OSSIA_INLINE ossia::value operator()(const domain_base<T>& value) const
  {
    return detail::domain_max_visitor_helper<domain_base<T>>{}(value);
  }

  OSSIA_INLINE ossia::value operator()(const vector_domain& value) const
  {
    return detail::domain_max_visitor_helper<vector_domain>{}(value);
  }

  template <std::size_t N>
  OSSIA_INLINE ossia::value operator()(const vecf_domain<N>& value) const
  {
    return detail::domain_max_visitor_helper<vecf_domain<N>>{}(value);
  }

  template <typename... T>
  OSSIA_INLINE ossia::value operator()(const T&...) const
  {
    return ossia::value{};
  }
};

struct domain_float_minmax_visitor
{
  using return_type = detail::float_minmax;
  template <typename T>
  OSSIA_INLINE return_type operator()(const domain_base<T>& value) const
  {
    return detail::domain_float_minmax_visitor_helper<domain_base<T>>{}(value);
  }

  OSSIA_INLINE return_type operator()(const vector_domain& value) const
  {
    return detail::domain_float_minmax_visitor_helper<vector_domain>{}(value);
  }

  template <std::size_t N>
  OSSIA_INLINE return_type operator()(const vecf_domain<N>& value) const
  {
    return detail::domain_float_minmax_visitor_helper<vecf_domain<N>>{}(value);
  }

  template <typename... T>
  OSSIA_INLINE return_type operator()(const T&...) const
  {
    return {};
  }
};
struct domain_set_min_visitor
{
  OSSIA_INLINE void operator()(domain_base<int32_t>& domain, int32_t incoming)
  {
    domain.min = incoming;
  }
  OSSIA_INLINE void operator()(domain_base<float>& domain, float incoming)
  {
    domain.min = incoming;
  }
  OSSIA_INLINE void operator()(domain_base<int32_t>& domain, float incoming)
  {
    domain.min = incoming;
  }
  OSSIA_INLINE void operator()(domain_base<float>& domain, int32_t incoming)
  {
    domain.min = incoming;
  }
  OSSIA_INLINE void operator()(domain_base<bool>& domain, bool incoming) { }
  OSSIA_INLINE void
  operator()(vector_domain& domain, const std::vector<ossia::value>& incoming)
  {
    domain.min = incoming;
  }

  template <std::size_t N>
  OSSIA_INLINE void operator()(vecf_domain<N>& domain, float incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
      domain.min[i] = incoming;
#endif
  }

  template <std::size_t N>
  OSSIA_INLINE void operator()(vecf_domain<N>& domain, int incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
      domain.min[i] = incoming;
#endif
  }

  template <std::size_t N>
  OSSIA_INLINE void
  operator()(vecf_domain<N>& domain, const std::array<float, N>& incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
      domain.min[i] = incoming[i];
#endif
  }

  template <std::size_t N>
  OSSIA_INLINE void
  operator()(vecf_domain<N>& domain, const std::vector<ossia::value>& incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    if(incoming.size() == N)
    {
      auto conv = ossia::convert<std::array<float, N>>(incoming);
      for(std::size_t i = 0; i < N; i++)
        domain.min[i] = conv[i];
    }
    else
    {
      for(std::size_t i = 0; i < N; i++)
        domain.min[i] = std::nullopt;
    }
#endif
  }

  template <typename T>
  OSSIA_INLINE void operator()(domain_base<ossia::value>& domain, const T& incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    domain.min = ossia::value{incoming};
#endif
  }

  // Removal cases
  // Here we could maybe allow a cast or something like this...
  // for e.g. int -> float
  template <typename... U>
  OSSIA_INLINE void operator()(domain_base<int32_t>& domain, U&&...)
  {
    domain.min = std::nullopt;
  }
  template <typename... U>
  OSSIA_INLINE void operator()(domain_base<float>& domain, U&&...)
  {
    domain.min = std::nullopt;
  }
  template <typename... U>
  OSSIA_INLINE void operator()(domain_base<bool>& domain, U&&...)
  {
  }
  template <typename... U>
  OSSIA_INLINE void operator()(vector_domain& domain, U&&...)
  {
    domain.min.clear();
  }
  template <std::size_t N, typename... U>
  OSSIA_INLINE void operator()(vecf_domain<N>& domain, U&&...)
  {
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
      domain.min[i] = std::nullopt;
#endif
  }
  template <typename... U>
  OSSIA_INLINE void operator()(domain_base<ossia::value>& domain, U&&...)
  {
    domain.min = std::nullopt;
  }

  template <typename T, typename... U>
  OSSIA_INLINE void operator()(const domain_base<T>& domain, U&&...)
  {
  }
};

struct domain_set_max_visitor
{
  OSSIA_INLINE void operator()(domain_base<int32_t>& domain, int32_t incoming)
  {
    domain.max = incoming;
  }
  OSSIA_INLINE void operator()(domain_base<float>& domain, float incoming)
  {
    domain.max = incoming;
  }
  OSSIA_INLINE void operator()(domain_base<int32_t>& domain, float incoming)
  {
    domain.max = incoming;
  }
  OSSIA_INLINE void operator()(domain_base<float>& domain, int32_t incoming)
  {
    domain.max = incoming;
  }
  OSSIA_INLINE void operator()(domain_base<bool>& domain, bool incoming) { }
  OSSIA_INLINE void
  operator()(vector_domain& domain, const std::vector<ossia::value>& incoming)
  {
    domain.max = incoming;
  }

  template <std::size_t N>
  OSSIA_INLINE void operator()(vecf_domain<N>& domain, float incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
      domain.max[i] = incoming;
#endif
  }

  template <std::size_t N>
  OSSIA_INLINE void operator()(vecf_domain<N>& domain, int incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
      domain.max[i] = incoming;
#endif
  }

  template <std::size_t N>
  OSSIA_INLINE void
  operator()(vecf_domain<N>& domain, const std::array<float, N>& incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
      domain.max[i] = incoming[i];
#endif
  }

  template <std::size_t N>
  OSSIA_INLINE void
  operator()(vecf_domain<N>& domain, const std::vector<ossia::value>& incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    if(incoming.size() == N)
    {
      auto conv = ossia::convert<std::array<float, N>>(incoming);
      for(std::size_t i = 0; i < N; i++)
        domain.max[i] = conv[i];
    }
    else
    {
      for(std::size_t i = 0; i < N; i++)
        domain.max[i] = std::nullopt;
    }
#endif
  }

  template <typename T>
  OSSIA_INLINE void operator()(domain_base<ossia::value>& domain, const T& incoming)
  {
#if !defined(OSSIA_FREESTANDING)
    domain.max = ossia::value{incoming};
#endif
  }

  // Removal cases
  template <typename... U>
  OSSIA_INLINE void operator()(domain_base<int32_t>& domain, U&&...)
  {
    domain.max = std::nullopt;
  }
  template <typename... U>
  OSSIA_INLINE void operator()(domain_base<float>& domain, U&&...)
  {
    domain.max = std::nullopt;
  }
  template <typename... U>
  OSSIA_INLINE void operator()(domain_base<bool>& domain, U&&...)
  {
  }
  template <typename... U>
  OSSIA_INLINE void operator()(vector_domain& domain, U&&...)
  {
    domain.max.clear();
  }
  template <std::size_t N, typename... U>
  OSSIA_INLINE void operator()(vecf_domain<N>& domain, U&&...)
  {
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
      domain.max[i] = std::nullopt;
#endif
  }
  template <typename... U>
  OSSIA_INLINE void operator()(domain_base<ossia::value>& domain, U&&...)
  {
    domain.max = std::nullopt;
  }

  template <typename T, typename... U>
  OSSIA_INLINE void operator()(const domain_base<T>& domain, U&&...)
  {
  }
};

struct domain_minmax_creation_visitor
{
  template <typename T>
  OSSIA_INLINE domain operator()(T min, T max)
  {
    return domain_base<T>(min, max);
  }

  OSSIA_INLINE domain operator()(bool min, bool max) { return domain_base<bool>{}; }

  template <std::size_t N>
  OSSIA_INLINE domain
  operator()(const std::array<float, N>& lhs, const std::array<float, N>& rhs)
  {
    return vecf_domain<N>(lhs, rhs);
  }

  OSSIA_INLINE domain
  operator()(const std::vector<ossia::value>& min, const std::vector<ossia::value>& max)
  {
    return vector_domain(min, max);
  }
  OSSIA_INLINE domain
  operator()(std::vector<ossia::value>&& min, std::vector<ossia::value>&& max)
  {
    return vector_domain(std::move(min), std::move(max));
  }

  OSSIA_INLINE domain operator()(impulse, impulse) { return domain_base<impulse>{}; }

  OSSIA_INLINE domain operator()(const std::string&, const std::string&)
  {
    return domain_base<std::string>();
  }
  OSSIA_INLINE domain operator()(const value_map_type&, const value_map_type&)
  {
    return domain{};
  }

  // Cases where there is no possible domain
  template <typename T, typename U>
  OSSIA_INLINE domain operator()(const T&, const U&)
  {
    return domain{};
  }
};

struct domain_min_creation_visitor
{
  template <typename T>
  OSSIA_INLINE domain operator()(T min)
  {
    domain_base<T> dom;
    dom.min = std::move(min);
    return dom;
  }

  OSSIA_INLINE domain operator()(bool min) { return domain_base<bool>{}; }

  template <std::size_t N>
  OSSIA_INLINE domain operator()(const std::array<float, N>& min)
  {
    vecf_domain<N> dom;
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
    {
      dom.min[i] = min[i];
    }
#endif
    return dom;
  }

  OSSIA_INLINE domain operator()(const std::vector<ossia::value>& min)
  {
    vector_domain dom;
    dom.min = min;
    return dom;
  }
  OSSIA_INLINE domain operator()(const value_map_type& min) { return {}; }
  OSSIA_INLINE domain operator()(std::vector<ossia::value>&& min)
  {
    return vector_domain(std::move(min), std::vector<ossia::value>(min.size()));
  }

  OSSIA_INLINE domain operator()(impulse) { return domain_base<impulse>{}; }

  OSSIA_INLINE domain operator()(const std::string&)
  {
    return domain_base<std::string>();
  }
};

struct domain_max_creation_visitor
{
  template <typename T>
  OSSIA_INLINE domain operator()(T max)
  {
    domain_base<T> dom;
    dom.max = std::move(max);
    return dom;
  }

  OSSIA_INLINE domain operator()(bool max) { return domain_base<bool>{}; }

  template <std::size_t N>
  OSSIA_INLINE domain operator()(const std::array<float, N>& max)
  {
    vecf_domain<N> dom;
#if !defined(OSSIA_FREESTANDING)
    for(std::size_t i = 0; i < N; i++)
    {
      dom.max[i] = max[i];
    }
#endif
    return dom;
  }

  OSSIA_INLINE domain operator()(const std::vector<ossia::value>& max)
  {
    vector_domain dom;
    dom.max = max;
    return dom;
  }
  OSSIA_INLINE domain operator()(std::vector<ossia::value>&& max)
  {
    return vector_domain(std::move(max), std::vector<ossia::value>(max.size()));
  }

  OSSIA_INLINE domain operator()(const value_map_type& min) { return {}; }
  OSSIA_INLINE domain operator()(impulse) { return domain_base<impulse>{}; }

  OSSIA_INLINE domain operator()(const std::string&)
  {
    return domain_base<std::string>();
  }
};

/**
 * @brief The domain_value_set_creation_visitor struct
 *
 * Used to create domains with a set of acceptable values instead of a min /
 * max
 */
struct domain_value_set_creation_visitor
{
  const ossia::small_vector<ossia::value, 2>& values;
  template <typename T>
  domain operator()(const T&)
  {
    // Cases where there is no possible domain
    domain_base<T> dom;
    for(auto& value : values)
    {
      if(auto r = value.target<T>())
        dom.values.push_back(*r);
    }
    return dom;
  }

  domain operator()(bool) { return domain_base<bool>{}; }

  domain operator()(const std::vector<ossia::value>& orig)
  {
    vector_domain res;
    ossia::flat_set<ossia::value> vals;

    for(const auto& value : values)
      vals.insert(value);

    res.values
        = std::vector<ossia::flat_set<ossia::value>>(orig.size(), std::move(vals));

    return res;
  }

  OSSIA_INLINE domain operator()(const value_map_type& min) { return {}; }

  template <std::size_t N>
  domain operator()(const std::array<float, N>&)
  {
    vecf_domain<N> res;
#if !defined(OSSIA_FREESTANDING)
    ossia::flat_set<float> vals;

    for(const auto& value : values)
      if(auto r = value.target<float>())
        vals.insert(*r);

    for(std::size_t i = 0; i < N - 1; i++)
      res.values[i] = vals;
    res.values[N - 1] = std::move(vals);
#endif
    return res;
  }

  domain operator()()
  {
    // TODO
    return {};
  }

  domain operator()(const impulse&) { return domain{}; }
};
}
