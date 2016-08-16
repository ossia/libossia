#pragma once
#include <ossia/detail/math.hpp>
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/optional.hpp>
#include <type_traits>

namespace ossia
{
struct clamp_visitor
{
  template<typename T, typename U, typename V>
  ossia::value operator()(const T& val, const U& min, const V& max)
  { return val; }

  ossia::value operator()(Int val, Int min, Int max)
  { return Int{ossia::clamp(val.value, min.value, max.value)}; }
  ossia::value operator()(Float val, Float min, Float max)
  { return Float{ossia::clamp(val.value, min.value, max.value)}; }
  ossia::value operator()(Char val, Char min, Char max)
  { return Char{ossia::clamp(val.value, min.value, max.value)}; }
  ossia::value operator()(Bool val, Bool min, Bool max)
  { return Bool{ossia::clamp(val.value, min.value, max.value)}; }
};

ossia::value clamp(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(clamp_visitor{}, val.v, min.v, max.v);
}

struct clamp_min_visitor
{
  template<typename T, typename U>
  ossia::value operator()(const T& val, const U& min)
  { return val; }

  ossia::value operator()(Int val, Int min)
  { return Int{ossia::clamp_min(val.value, min.value)}; }
  ossia::value operator()(Float val, Float min)
  { return Float{ossia::clamp_min(val.value, min.value)}; }
  ossia::value operator()(Char val, Char min)
  { return Char{ossia::clamp_min(val.value, min.value)}; }
  ossia::value operator()(Bool val, Bool min)
  { return Bool{ossia::clamp_min(val.value, min.value)}; }
};

ossia::value clamp_min(const ossia::value& val, const ossia::value& min)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid())
    return val;
  return eggs::variants::apply(clamp_min_visitor{}, val.v, min.v);
}


struct clamp_max_visitor
{
  template<typename T, typename U>
  ossia::value operator()(const T& val, const U& max)
  { return val; }

  ossia::value operator()(Int val, Int max)
  { return Int{ossia::clamp_max(val.value, max.value)}; }
  ossia::value operator()(Float val, Float max)
  { return Float{ossia::clamp_max(val.value, max.value)}; }
  ossia::value operator()(Char val, Char max)
  { return Char{ossia::clamp_max(val.value, max.value)}; }
  ossia::value operator()(Bool val, Bool max)
  { return Bool{ossia::clamp_max(val.value, max.value)}; }
};

ossia::value clamp_max(const ossia::value& val, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe max ?
  if(!max.valid())
    return val;
  return eggs::variants::apply(clamp_max_visitor{}, val.v, max.v);
}


struct wrap_visitor
{
  template<typename T, typename U, typename V>
  ossia::value operator()(const T& val, const U& min, const V& max)
  { return val; }

  ossia::value operator()(Int val, Int min, Int max)
  { return Int{ossia::wrap(val.value, min.value, max.value)}; }
  ossia::value operator()(Float val, Float min, Float max)
  { return Float{ossia::wrap(val.value, min.value, max.value)}; }
  ossia::value operator()(Char val, Char min, Char max)
  { return Char{ossia::wrap(val.value, min.value, max.value)}; }
  ossia::value operator()(Bool val, Bool min, Bool max)
  { return Bool{ossia::wrap(val.value, min.value, max.value)}; }
};

ossia::value wrap(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(wrap_visitor{}, val.v, min.v, max.v);
}

struct fold_visitor
{
  template<typename T, typename U, typename V>
  ossia::value operator()(const T& val, const U& min, const V& max)
  { return val; }

  ossia::value operator()(Int val, Int min, Int max)
  { return Int{ossia::fold(val.value, min.value, max.value)}; }
  ossia::value operator()(Float val, Float min, Float max)
  { return Float{ossia::fold(val.value, min.value, max.value)}; }
  ossia::value operator()(Char val, Char min, Char max)
  { return Char{ossia::fold(val.value, min.value, max.value)}; }
  ossia::value operator()(Bool val, Bool min, Bool max)
  { return Bool{ossia::fold(val.value, min.value, max.value)}; }
};

ossia::value fold(const ossia::value& val, const ossia::value& min, const ossia::value& max)
{
  if(!val.valid())
    return {}; // TODO maybe min ?
  if(!min.valid() || !max.valid())
    return val;
  return eggs::variants::apply(fold_visitor{}, val.v, min.v, max.v);
}

namespace net
{
template <typename T>
struct domain_base
{
  using value_type = typename value_trait<T>::value_type;
  boost::optional<value_type> min;
  boost::optional<value_type> max;
  boost::container::flat_set<value_type> values;

  domain_base() = default;
  domain_base(value_type min_v, value_type max_v) : min{min_v}, max{max_v}
  {
  }

  value clamp(bounding_mode b, const T& val) const
  {
    if (b == bounding_mode::FREE)
      return val;

    if (values.empty())
    {
      bool has_min = bool(min);
      bool has_max = bool(max);
      if (has_min && has_max)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
            return T(ossia::clamp(val.value, *min, *max));
          case bounding_mode::WRAP:
            return T(ossia::wrap(val.value, *min, *max));
          case bounding_mode::FOLD:
            return T(ossia::fold(val.value, *min, *max));
          default:
            return val;
        }
      }
      else if (has_min)
      {
        if (b == bounding_mode::CLIP)
          return T(val.value < *min ? *min : val.value);
        else
          return val;
      }
      else if (has_max)
      {
        if (b == bounding_mode::CLIP)
          return T(val.value > *max ? *max : val.value);
        else
          return val;
      }
      else
      {
        return val;
      }
    }
    else
    {
      // Return a valid value only if it is in the given values
      auto it = values.find(val.value);
      if (it != values.end())
      {
        return T(*it);
      }
      else
      {
        return ossia::value{};
      }

      /* Alternative : return the closest element to value
      auto it = values.lower_bound(val.value);
      if(it != values.end())
      {
          if(it == values.begin())
          {
              // The closest is the first element
              return T(it);
          }
          else
          {
              // Find the closest element between this one and the previous.
              auto prev = it - 1;
              auto prev_diff = std::abs(val.value - *prev);
              auto cur_diff = std::abs(val.value - *it);
              return prev_diff > cur_diff ? *it : *prev;
          }
      }
      else if(it == values.end())
      {
          // Closest element is the latest
          return T(*values.rbegin());
      }
      */
    }
  }
};

template <>
struct domain_base<Impulse>
{
  value clamp(bounding_mode b, const Impulse& val) const
  {
    return val;
  }
};

template <>
struct domain_base<Behavior>
{
  value clamp(bounding_mode b, const Behavior& val) const
  {
    return val;
  }
};

template <>
struct domain_base<Destination>
{
  value clamp(bounding_mode b, const Destination& val) const
  {
    return val;
  }
};

template <>
struct domain_base<String>
{
  boost::container::flat_set<std::string> values;
  value clamp(bounding_mode b, const String& val) const
  {
    if (values.empty())
    {
      return val;
    }
    else
    {
      auto it = values.find(val.value);
      if (it != values.end())
      {
        return String(*it);
      }
      else
      {
        return ossia::value{};
      }
    }
  }
};

template <>
struct domain_base<Tuple>
{
  boost::optional<ossia::value> min;
  boost::optional<ossia::value> max;

  value clamp(bounding_mode b, const Tuple& val) const
  {
    if (b == bounding_mode::FREE)
      return val;

    bool has_min = bool(min);
    bool has_max = bool(max);
    if (has_min && has_max)
    {
      ossia::Tuple res;
      res.value.reserve(val.value.size());

      switch (b)
      {
        case bounding_mode::CLIP:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp(v, *min, *max));
          }
          break;
        case bounding_mode::WRAP:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::wrap(v, *min, *max));
          }
          break;
        case bounding_mode::FOLD:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::fold(v, *min, *max));
          }
          break;
        default:
          return val;
      }

      return res;
    }
    else if (has_min)
    {
      if (b == bounding_mode::CLIP)
      {
        ossia::Tuple res;
        for(auto& v : val.value)
        {
          res.value.push_back(ossia::clamp_min(v, *min));
        }
        return res;
      }
      else
      {
        return val;
      }
    }
    else if (has_max)
    {
      if (b == bounding_mode::CLIP)
      {
        ossia::Tuple res;
        for(auto& v : val.value)
        {
          res.value.push_back(ossia::clamp_max(v, *max));
        }
        return res;
      }
      else
      {
        return val;
      }
    }
    else
    {
      return val;
    }
  }
};
template <int N>
struct domain_base<Vec<float, N>>
{
  value clamp(bounding_mode b, const Vec<float, N>& val) const
  {
    // TODO
    return val;
  }
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
                    domain_base<Behavior>>;
}
}
