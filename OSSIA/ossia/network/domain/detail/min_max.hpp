#pragma once
#include <ossia/network/domain/domain_base.hpp>

namespace ossia
{
namespace net
{
namespace detail
{
/// Min ///
template<typename T, typename = void>
struct domain_min_visitor_helper
{
  template <typename U>
  ossia::value operator()(const U&) const
  { return ossia::value{}; }
};

template<typename T>
struct domain_min_visitor_helper<T, void_t<decltype(std::declval<T>().min)>>
{
  ossia::value operator()(const T& value) const
  {
    if(value.min)
      return value::make<typename T::ossia_type>(value.min.get());
    else
      return ossia::value{};
  }
};

template<>
struct domain_min_visitor_helper<domain_base<ossia::value>>
{
  ossia::value operator()(const domain_base<ossia::value>& value) const
  {
    // TODO for this case, it would maybe be better to
    // use the empty state of value instead of a boost::optional ?
    if(value.min)
      return value.min.get();
    else
      return ossia::value{};
  }
};


/// Max ///
template<typename T, typename = void>
struct domain_max_visitor_helper
{
  template <typename U>
  ossia::value operator()(const U&) const
  { return ossia::value{}; }
};

template<typename T>
struct domain_max_visitor_helper<T, void_t<decltype(std::declval<T>().max)>>
{
  ossia::value operator()(const T& value) const
  {
    if(value.max)
      return value::make<typename T::ossia_type>(value.max.get());
    else
      return ossia::value{};
  }
};

template<>
struct domain_max_visitor_helper<domain_base<ossia::value>>
{
  ossia::value operator()(const domain_base<ossia::value>& value) const
  {
    // TODO for this case, it would maybe be better to
    // use the empty state of value instead of a boost::optional ?
    if(value.max)
      return value.max.get();
    else
      return ossia::value{};
  }
};

}

struct domain_min_visitor
{
  template<typename T>
  ossia::value operator()(const domain_base<T>& value) const
  { return detail::domain_min_visitor_helper<domain_base<T>>{}(value); }

  template <typename... T>
  ossia::value operator()(const T&...) const
  { return ossia::value{}; }
};

struct domain_max_visitor
{
  template<typename T>
  ossia::value operator()(const domain_base<T>& value) const
  { return detail::domain_max_visitor_helper<domain_base<T>>{}(value); }

  template <typename... T>
  ossia::value operator()(const T&...) const
  { return ossia::value{}; }
};

struct domain_set_min_visitor
{
  void operator()(domain_base<Int>& domain, Int incoming)
  { domain.min = incoming.value; }
  void operator()(domain_base<Float>& domain, Float incoming)
  { domain.min = incoming.value; }
  void operator()(domain_base<Char>& domain, Char incoming)
  { domain.min = incoming.value; }
  void operator()(domain_base<Bool>& domain, Bool incoming)
  { domain.min = incoming.value; }
  void operator()(domain_base<Tuple>& domain, const Tuple& incoming)
  { domain.min = incoming.value; }

  template<std::size_t N>
  void operator()(domain_base<Vec<float, N>>& domain, const Vec<float, N>& incoming)
  { domain.min = incoming; }

  template<typename T>
  void operator()(domain_base<ossia::value>& domain, const T& incoming)
  { domain.min = ossia::value{incoming}; }

  // Removal cases
  // Here we could maybe allow a cast or something like this...
  // for e.g. int -> float
  template <typename... U>
  void operator()(domain_base<Int>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<Float>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<Char>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<Bool>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<Tuple>& domain, U&&...)
  { domain.min.reset(); }
  template<std::size_t N, typename... U>
  void operator()(domain_base<Vec<float, N>>& domain, U&&...)
  { domain.min.reset(); }
  template <typename... U>
  void operator()(domain_base<ossia::value>& domain, U&&...)
  { domain.min.reset(); }

  template <typename T, typename... U>
  void operator()(const domain_base<T>& domain, U&&...)
  {
  }
};

struct domain_set_max_visitor
{
  void operator()(domain_base<Int>& domain, Int incoming)
  { domain.max = incoming.value; }
  void operator()(domain_base<Float>& domain, Float incoming)
  { domain.max = incoming.value; }
  void operator()(domain_base<Char>& domain, Char incoming)
  { domain.max = incoming.value; }
  void operator()(domain_base<Bool>& domain, Bool incoming)
  { domain.max = incoming.value; }
  void operator()(domain_base<Tuple>& domain, const Tuple& incoming)
  { domain.max = incoming.value; }

  template<std::size_t N>
  void operator()(domain_base<Vec<float, N>>& domain, const Vec<float, N>& incoming)
  { domain.max = incoming; }

  template<typename T>
  void operator()(domain_base<ossia::value>& domain, const T& incoming)
  { domain.max = ossia::value{incoming}; }

  // Removal cases
  template <typename... U>
  void operator()(domain_base<Int>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<Float>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<Char>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<Bool>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<Tuple>& domain, U&&...)
  { domain.max.reset(); }
  template<std::size_t N, typename... U>
  void operator()(domain_base<Vec<float, N>>& domain, U&&...)
  { domain.max.reset(); }
  template <typename... U>
  void operator()(domain_base<ossia::value>& domain, U&&...)
  { domain.max.reset(); }

  template <typename T, typename... U>
  void operator()(const domain_base<T>& domain, U&&...)
  {
  }
};


struct domain_minmax_creation_visitor
{
  template <typename T>
  domain operator()(T min, T max)
  { return domain_base<T>(min, max); }

  domain operator()(const Tuple& min, const Tuple& max)
  { return domain_base<Tuple>(min, max); }
  domain operator()(Tuple&& min, Tuple&& max)
  { return domain_base<Tuple>(std::move(min), std::move(max)); }

  domain operator()(Impulse, Impulse)
  { return domain_base<Impulse>{}; }

  domain operator()(const String&, const String&)
  { return domain_base<String>(); }


  // Cases where there is no possible domain
  template <typename T, typename U>
  domain operator()(const T&, const U&)
  { return domain{}; }
  domain operator()(const Destination&, const Destination&)
  { return domain{}; }
};


/**
 * @brief The domain_value_set_creation_visitor struct
 *
 * Used to create domains with a set of acceptable values instead of a min / max
 */
struct domain_value_set_creation_visitor
{
  const boost::container::small_vector<ossia::value, 2>& values;
  template <typename T>
  domain operator()(const T&)
  {
    // Cases where there is no possible domain
    domain_base<T> dom;
    for(auto& value : values)
    {
      if(auto r = value.target<T>())
        dom.values.insert(r->value);
    }
    return dom;
  }

  domain operator()(const Destination&)
  { return domain{}; }
  domain operator()(const Impulse&)
  { return domain{}; }
};
}
}
