#pragma once
#include <ossia/network/domain/numeric_domain.hpp>
namespace ossia
{

/**
 * Applying a domain value by value to arrays
 */
template<typename Domain>
struct array_clamp
{
  const Domain& domain;

  ossia::value operator()(bounding_mode b, const Tuple& val) const
  {
      if (b == bounding_mode::FREE)
        return val;

      // We handle values by checking component by component
      const auto& values = domain.values;
      if(values.empty())
      {
        const auto N = v.value.size();
        const bool has_min = bool(domain.min);
        const bool has_max = bool(domain.max);
        if (has_min && has_max)
        {
          const Tuple& min = domain.min.get();
          const Tuple& max = domain.max.get();

          if(N != min.value.size() || N != max.value.size())
            return {};

          ossia::Tuple res;
          res.value.resize(N);
          switch (b)
          {
            case bounding_mode::CLIP:
              for(int i = 0; i < N; i++) res[i] = ossia::clamp(val.value[i], min.value[i], max.value[i]);
              break;
            case bounding_mode::WRAP:
              for(int i = 0; i < N; i++) res[i] = ossia::wrap(val.value[i], min.value[i], max.value[i]);
              break;
            case bounding_mode::FOLD:
              for(int i = 0; i < N; i++) res[i] = ossia::fold(val.value[i], min.value[i], max.value[i]);
              break;
            case bounding_mode::LOW:
              for(int i = 0; i < N; i++) res[i] = ossia::clamp_min(val.value[i], min.value[i]);
              break;
            case bounding_mode::HIGH:
              for(int i = 0; i < N; i++) res[i] = ossia::clamp_max(val.value[i], max.value[i]);
              break;
            default:
          }
          return res;
        }
        else if (has_min)
        {
          ossia::Tuple res;
          res.value.resize(N);
          switch(b)
          {
            case bounding_mode::CLIP:
            case bounding_mode::LOW:
              for(int i = 0; i < N; i++) res[i] = ossia::clamp_min(val.value[i], min.value[i]);
            default:
          }
          return res;
        }
        else if (has_max)
        {
          ossia::Tuple res;
          res.value.resize(N);
          switch(b)
          {
            case bounding_mode::CLIP:
            case bounding_mode::HIGH:
              for(int i = 0; i < N; i++) res[i] = ossia::clamp_max(val.value[i], max.value[i]);
            default:
          }
          return res;
        }

        return val;
      }
      else
      {
        auto it = values.find(val);
        return (it != values.end())
                ? val
                : ossia::value{};
      }
  }

  template<int N>
  ossia::value operator()(bounding_mode b, const Vec<float, N>& val) const
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
          const Vec<float, N>& min = domain.min.get();
          const Vec<float, N>& max = domain.max.get();
          switch (b)
          {
            case bounding_mode::CLIP:
              for(int i = 0; i < N; i++) val.value[i] = ossia::clamp(val.value[i], min.value[i], max.value[i]);
              break;
            case bounding_mode::WRAP:
              for(int i = 0; i < N; i++) val.value[i] = ossia::wrap(val.value[i], min.value[i], max.value[i]);
              break;
            case bounding_mode::FOLD:
              for(int i = 0; i < N; i++) val.value[i] = ossia::fold(val.value[i], min.value[i], max.value[i]);
              break;
            case bounding_mode::LOW:
              for(int i = 0; i < N; i++) val.value[i] = ossia::clamp_min(val.value[i], min.value[i]);
              break;
            case bounding_mode::HIGH:
              for(int i = 0; i < N; i++) val.value[i] = ossia::clamp_max(val.value[i], max.value[i]);
              break;
            default:
          }
        }
        else if (has_min)
        {
          switch(b)
          {
            case bounding_mode::CLIP:
            case bounding_mode::LOW:
              for(int i = 0; i < N; i++) val.value[i] = ossia::clamp_min(val.value[i], min.value[i]);
            default:
          }
        }
        else if (has_max)
        {
          switch(b)
          {
            case bounding_mode::CLIP:
            case bounding_mode::HIGH:
              for(int i = 0; i < N; i++) val.value[i] = ossia::clamp_max(val.value[i], max.value[i]);
            default:
          }
        }

        return val;
      }
      else
      {
        auto it = values.find(val);
        return (it != values.end())
                ? v
                : ossia::value{};
      }
  }

}
