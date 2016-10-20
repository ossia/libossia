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
struct OSSIA_EXPORT domain_base;

template<typename Domain>
struct numeric_clamp
{
  const Domain& domain;

  template<typename U>
  value operator()(bounding_mode b, U&& val) const
  {
    using T = typename Domain::ossia_type;

    if (b == bounding_mode::FREE)
      return std::forward<U>(val);

    if (domain.values.empty())
    {
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        const auto min = domain.min.get();
        const auto max = domain.max.get();
        switch (b)
        {
          case bounding_mode::CLIP:
            return T(ossia::clamp(std::forward<U>(val).value, min, max));
          case bounding_mode::WRAP:
            return T(ossia::wrap(std::forward<U>(val).value, min, max));
          case bounding_mode::FOLD:
            return T(ossia::fold(std::forward<U>(val).value, min, max));
          case bounding_mode::LOW:
            return T(ossia::clamp_min(std::forward<U>(val).value, min));
          case bounding_mode::HIGH:
            return T(ossia::clamp_max(std::forward<U>(val).value, max));
          default:
            return std::forward<U>(val);
        }
      }
      else if (has_min)
      {
        const auto min = domain.min.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            return T(ossia::clamp_min(std::forward<U>(val).value, min));
          default:
            return std::forward<U>(val);
        }
      }
      else if (has_max)
      {
        const auto max = domain.max.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            return T(ossia::clamp_max(val.value, max));
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
      auto it = domain.values.find(val.value);
      if (it != domain.values.end())
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

  /**
   * Fast algorithm for applying a numeric domain to a Vec<float>
   */
  template<int N>
  ossia::value operator()(bounding_mode b, Vec<float, N> val) const
  {
    if (b == bounding_mode::FREE)
      return val;

    // We handle values by checking component by component
    const auto& values = domain.values;
    if(values.empty())
    {
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        const auto min = domain.min.get();
        const auto max = domain.max.get();
        switch (b)
        {
          case bounding_mode::CLIP:
            for(int i = 0; i < N; i++) val.value[i] = ossia::clamp(val.value[i], min, max);
            break;
          case bounding_mode::WRAP:
            for(int i = 0; i < N; i++) val.value[i] = ossia::wrap(val.value[i], min, max);
            break;
          case bounding_mode::FOLD:
            for(int i = 0; i < N; i++) val.value[i] = ossia::fold(val.value[i], min, max);
            break;
          case bounding_mode::LOW:
            for(int i = 0; i < N; i++) val.value[i] = ossia::clamp_min(val.value[i], min);
            break;
          case bounding_mode::HIGH:
            for(int i = 0; i < N; i++) val.value[i] = ossia::clamp_max(val.value[i], max);
            break;
          default:
            break;
        }
      }
      else if (has_min)
      {
        const auto min = domain.min.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            for(int i = 0; i < N; i++) val.value[i] = ossia::clamp_min(val.value[i], min);
          default:
            break;
        }
      }
      else if (has_max)
      {
        const auto max = domain.max.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(int i = 0; i < N; i++) val.value[i] = ossia::clamp_max(val.value[i], max);
          default:
            break;
        }
      }

      return val;
    }
    else
    {
      for(int i = 0; i < N; i++)
      {
        // Return a valid value only if it is in the given values
        auto it = values.find(val[i]);
        if (it == values.end())
        {
          return {};
        }
      }

      return val;
    }
  }
};

}
}
