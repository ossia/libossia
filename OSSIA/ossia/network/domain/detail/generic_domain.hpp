#pragma once
#include <ossia/network/domain/domain_base.hpp>
namespace ossia
{

/**
 * Applying a domain without caring about the underlying type
 */
struct generic_clamp
{
  const net::domain_base<ossia::value>& domain;

  ossia::value operator()(bounding_mode b, const ossia::value& v) const
  {
    if (b == bounding_mode::FREE)
      return v;

    const auto& values = domain.values;
    if(values.empty())
    {
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        const auto& min = domain.min.get();
        const auto& max = domain.max.get();
        switch (b)
        {
          case bounding_mode::CLIP:
            return ossia::clamp(v, min, max);
          case bounding_mode::WRAP:
            return ossia::wrap(v, min, max);
          case bounding_mode::FOLD:
            return ossia::fold(v, min, max);
          case bounding_mode::LOW:
            return ossia::clamp_min(v, min);
          case bounding_mode::HIGH:
            return ossia::clamp_max(v, max);
          default:
            break;
        }
      }
      else if (has_min)
      {
        const auto& min = domain.min.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            return ossia::clamp_min(v, min);
          default:
            break;
        }
      }
      else if (has_max)
      {
        const auto& max = domain.max.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            return ossia::clamp_max(v, max);
          default:
            break;
        }
      }

      return v;
    }
    else
    {
      // Return a valid value only if it is in the given values
      auto it = values.find(v);
      return (it != values.end())
          ? v
          : ossia::value{};
    }
  }

  ossia::value operator()(bounding_mode b, ossia::value&& v) const
  {
    if (b == bounding_mode::FREE)
      return v;

    const auto& values = domain.values;
    if(values.empty())
    {
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        const auto& min = domain.min.get();
        const auto& max = domain.max.get();
        switch (b)
        {
          case bounding_mode::CLIP:
            return ossia::clamp(std::move(v), min, max);
          case bounding_mode::WRAP:
            return ossia::wrap(std::move(v), min, max);
          case bounding_mode::FOLD:
            return ossia::fold(std::move(v), min, max);
          case bounding_mode::LOW:
            return ossia::clamp_min(std::move(v), min);
          case bounding_mode::HIGH:
            return ossia::clamp_max(std::move(v), max);
          default:
            break;
        }
      }
      else if (has_min)
      {
        const auto& min = domain.min.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            return ossia::clamp_min(std::move(v), min);
          default:
            break;
        }
      }
      else if (has_max)
      {
        const auto& max = domain.max.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            return ossia::clamp_max(std::move(v), max);
          default:
            break;
        }
      }

      return std::move(v);
    }
    else
    {
      // Return a valid value only if it is in the given values
      auto it = values.find(v);
      return (it != values.end())
          ? std::move(v)
          : ossia::value{};
    }
  }


  // TODO should we have the same optimization for Vec?
  // This will clamp the Tuple component by component with the given value.
  value operator()(bounding_mode b, const Tuple& val) const
  {
    if (b == bounding_mode::FREE)
      return val;

    const auto& values = domain.values;
    if(values.empty())
    {
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        ossia::Tuple res;
        res.value.reserve(val.value.size());
        const auto& min = domain.min.get();
        const auto& max = domain.max.get();

        switch (b)
        {
          case bounding_mode::CLIP:
            for(auto& v : val.value) res.value.push_back(ossia::clamp(v, min, max));
            break;
          case bounding_mode::WRAP:
            for(auto& v : val.value) res.value.push_back(ossia::wrap(v, min, max));
            break;
          case bounding_mode::FOLD:
            for(auto& v : val.value) res.value.push_back(ossia::fold(v, min, max));
            break;
          case bounding_mode::LOW:
            for(auto& v : val.value) res.value.push_back(ossia::clamp_min(v, min));
            break;
          case bounding_mode::HIGH:
            for(auto& v : val.value) res.value.push_back(ossia::clamp_max(v, max));
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
            const auto& min = domain.min.get();
            ossia::Tuple res;
            res.value.reserve(val.value.size());
            for(auto& v : val.value)
            {
              res.value.push_back(ossia::clamp_min(v, min));
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
            const auto& max = domain.max.get();
            ossia::Tuple res;
            res.value.reserve(val.value.size());
            for(auto& v : val.value)
            {
              res.value.push_back(ossia::clamp_max(v, max));
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
    else
    {
      for(auto& v : val.value)
      {
        // Return a valid value only if all the subvalues are in the given values
        auto it = values.find(v);
        if (it == values.end())
        {
          return {};
        }
      }
      return val;
    }
  }

  value operator()(bounding_mode b, Tuple&& val) const
  {
    if (b == bounding_mode::FREE)
      return std::move(val);

    const auto& values = domain.values;
    if(values.empty())
    {
      const bool has_min = bool(domain.min);
      const bool has_max = bool(domain.max);
      if (has_min && has_max)
      {
        const auto& min = domain.min.get();
        const auto& max = domain.max.get();
        switch (b)
        {
          case bounding_mode::CLIP:
            for(auto& v : val.value) v = ossia::clamp(v, min, max);
            break;
          case bounding_mode::WRAP:
            for(auto& v : val.value) v = ossia::wrap(v, min, max);
            break;
          case bounding_mode::FOLD:
            for(auto& v : val.value) v = ossia::fold(v, min, max);
            break;
          case bounding_mode::LOW:
            for(auto& v : val.value) v = ossia::clamp_min(v, min);
            break;
          case bounding_mode::HIGH:
            for(auto& v : val.value) v = ossia::clamp_max(v, max);
            break;
          default:
            return std::move(val);
        }

        return std::move(val);
      }
      else if (has_min)
      {
        const auto& min = domain.min.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
          {
            for(auto& v : val.value) v = ossia::clamp_min(v, min);
            return std::move(val);
          }
          default:
            return std::move(val);
        }
      }
      else if (has_max)
      {
        const auto& max = domain.max.get();
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
          {
            for(auto& v : val.value) v = ossia::clamp_max(v, max);
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
    else
    {
      for(auto& v : val.value)
      {
        // Return a valid value only if it is in the given values
        auto it = values.find(v);
        if (it == values.end())
        {
          // TODO should we remove the "bad" value instead ?
          // Or maybeclamping to the closest value
          // would make most sense.
          return {};
        }
      }
      return std::move(val);
    }
  }
};
}
