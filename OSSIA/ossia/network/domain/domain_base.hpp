#pragma once
#include <ossia/detail/math.hpp>
#include <ossia/editor/value/value.hpp>
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
    if (b == bounding_mode::FREE)
      return std::forward<U>(val);

    if (values.empty())
    {
      bool has_min = bool(min);
      bool has_max = bool(max);
      if (has_min && has_max)
      {
        switch (b)
        {
          case bounding_mode::CLIP:
            return T(ossia::clamp(std::forward<U>(val).value, *min, *max));
          case bounding_mode::WRAP:
            return T(ossia::wrap(std::forward<U>(val).value, *min, *max));
          case bounding_mode::FOLD:
            return T(ossia::fold(std::forward<U>(val).value, *min, *max));
          case bounding_mode::LOW:
            return T(ossia::clamp_min(std::forward<U>(val).value, *min));
          case bounding_mode::HIGH:
            return T(ossia::clamp_max(std::forward<U>(val).value, *max));
          default:
            return std::forward<U>(val);
        }
      }
      else if (has_min)
      {
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            return T(ossia::clamp_min(std::forward<U>(val).value, *min));
          default:
            return std::forward<U>(val);
        }
      }
      else if (has_max)
      {
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            return T(ossia::clamp_max(val.value, *max));
          default:
            return std::forward<U>(val);
        }
      }
      else
      {
        return std::forward<U>(val);
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
struct OSSIA_EXPORT domain_base<Tuple>
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
        case bounding_mode::LOW:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp_min(v, *min));
          }
          break;
        case bounding_mode::HIGH:
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp_max(v, *max));
          }
          break;
        default:
          return val;
      }

      return res;
    }
    else if (has_min)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
        {
          ossia::Tuple res;
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp_min(v, *min));
          }
          return res;
        }
        default:
          return val;
      }
    }
    else if (has_max)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
        {
          ossia::Tuple res;
          for(auto& v : val.value)
          {
            res.value.push_back(ossia::clamp_max(v, *max));
          }
          return res;
        }
        default:
          return val;
      }
    }
    else
    {
      return val;
    }
  }

  value clamp(bounding_mode b, Tuple&& val) const
  {
    if (b == bounding_mode::FREE)
      return std::move(val);

    bool has_min = bool(min);
    bool has_max = bool(max);
    if (has_min && has_max)
    {
      switch (b)
      {
        case bounding_mode::CLIP:
          for(auto& v : val.value)
          {
            v = ossia::clamp(v, *min, *max);
          }
          break;
        case bounding_mode::WRAP:
          for(auto& v : val.value)
          {
            v = ossia::wrap(v, *min, *max);
          }
          break;
        case bounding_mode::FOLD:
          for(auto& v : val.value)
          {
            v = ossia::fold(v, *min, *max);
          }
          break;
        case bounding_mode::LOW:
          for(auto& v : val.value)
          {
            v = ossia::clamp_min(v, *min);
          }
          break;
        case bounding_mode::HIGH:
          for(auto& v : val.value)
          {
            v = ossia::clamp_max(v, *max);
          }
          break;
        default:
          return std::move(val);
      }

      return std::move(val);
    }
    else if (has_min)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
        {
          for(auto& v : val.value)
          {
            v = ossia::clamp_min(v, *min);
          }
          return std::move(val);
        }
        default:
          return std::move(val);
      }
    }
    else if (has_max)
    {
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
        {
          for(auto& v : val.value)
          {
            v = ossia::clamp_min(v, *min);
          }
          return std::move(val);
        }
        default:
          return std::move(val);
      }
    }
    else
    {
      return std::move(val);
    }
  }
};
template <int N>
struct OSSIA_EXPORT domain_base<Vec<float, N>>
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
