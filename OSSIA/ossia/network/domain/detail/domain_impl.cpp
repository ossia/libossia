#include <ossia/network/domain/detail/apply_domain.hpp>
#include <ossia/network/domain/domain_conversion.hpp>
#include <fmt/format.h>
namespace ossia
{
namespace net
{

value domain::get_min() const
{ return net::get_min(*this); }
value domain::get_max() const
{ return net::get_max(*this); }

void domain::set_min(const ossia::value& val)
{ return net::set_min(*this, val); }
void domain::set_max(const ossia::value& val)
{ return net::set_max(*this, val); }

value domain::apply(bounding_mode b, const ossia::value& val) const
{ return net::apply_domain(*this, b, val); }
value domain::apply(bounding_mode b, ossia::value&& val) const
{ return net::apply_domain(*this, b, std::move(val)); }

struct domain_prettyprint_visitor
{
  fmt::MemoryWriter& writer;
  template<typename Domain>
  void operator()(Domain& dom)
  {
    if(dom.min)
    {
      writer << "min:";
      writer << *dom.min;
    }

    if(dom.max)
    {
      writer << "max:";
      writer << *dom.max;
    }

  }

  void operator()(domain_base<bool>& dom)
  {
    writer << true << "bool";
  }
  void operator()(domain_base<Impulse>& dom)
  {
    writer << "impulse";
  }

  void operator()(domain_base<std::string>& dom)
  {
    writer << "string";
  }

  void operator()(domain_base<ossia::value>& dom)
  {
    writer << "generic";
  }

  void operator()(domain_base<std::vector<ossia::value>>& dom)
  {
    writer << "tuple";
  }
};

std::string domain::to_pretty_string() const
{
  fmt::MemoryWriter s;
  eggs::variants::apply(domain_prettyprint_visitor{s}, (domain_base_variant&)*this);
  return s.str();
}

template<typename Domain>
template<typename U>
ossia::value numeric_clamp<Domain>::operator()(bounding_mode b, U&& val) const
{
  using T = typename Domain::value_type;

  if (b == bounding_mode::FREE)
    return std::forward<U>(val);

  if (domain.values.empty())
  {
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const auto min = *domain.min;
      const auto max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          return T(ossia::clamp(std::forward<U>(val), min, max));
        case bounding_mode::WRAP:
          return T(ossia::wrap(std::forward<U>(val), min, max));
        case bounding_mode::FOLD:
          return T(ossia::fold(std::forward<U>(val), min, max));
        case bounding_mode::LOW:
          return T(ossia::clamp_min(std::forward<U>(val), min));
        case bounding_mode::HIGH:
          return T(ossia::clamp_max(std::forward<U>(val), max));
        default:
          break;
      }
    }
    else if (has_min)
    {
      const auto min = *domain.min;
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          return T(ossia::clamp_min(std::forward<U>(val), min));
        default:
          break;
      }
    }
    else if (has_max)
    {
      const auto max = *domain.max;
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          return T(ossia::clamp_max(val, max));
        default:
          break;
      }
    }

    return std::forward<U>(val);
  }
  else
  {
    // Return a valid value only if it is in the given values
    auto it = domain.values.find(val);
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
template<typename T>
template<std::size_t N>
ossia::value numeric_clamp<T>::operator()(bounding_mode b, std::array<float, N> val) const
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
      const float min = *domain.min;
      const float max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp(val[i], min, max);
          break;
        case bounding_mode::WRAP:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::wrap(val[i], min, max);
          break;
        case bounding_mode::FOLD:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::fold(val[i], min, max);
          break;
        case bounding_mode::LOW:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_min(val[i], min);
          break;
        case bounding_mode::HIGH:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_max(val[i], max);
          break;
        default:
          break;
      }
    }
    else if (has_min)
    {
      const float min = *domain.min;
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_min(val[i], min);
        default:
          break;
      }
    }
    else if (has_max)
    {
      const float max = *domain.max;
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_max(val[i], max);
        default:
          break;
      }
    }

    return val;
  }
  else
  {
    for(std::size_t i = 0; i < N; i++)
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

value tuple_clamp::operator()(bounding_mode b, const std::vector<ossia::value>& val) const
{
  if (b == bounding_mode::FREE)
    return val;

  // We handle values by checking component by component
  const auto& values = domain.values;
  if(values.empty())
  {
    const auto N = val.size();
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const std::vector<ossia::value>& min = *domain.min;
      const std::vector<ossia::value>& max = *domain.max;
      if(min.size() == 1 && max.size() == 1)
      {
        std::vector<ossia::value> res;
        res.resize(N);

        const auto min_v = min[0];
        const auto max_v = max[0];

        switch (b)
        {
          case bounding_mode::CLIP:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp(val[i], min_v, max_v);
            break;
          case bounding_mode::WRAP:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::wrap(val[i], min_v, max_v);
            break;
          case bounding_mode::FOLD:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::fold(val[i], min_v, max_v);
            break;
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp_min(val[i], min_v);
            break;
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp_max(val[i], max_v);
            break;
          default:
            break;
        }
        return res;
      }
      else if(N != min.size() || N != max.size())
      {
        return {};
      }
      else
      {
        std::vector<ossia::value> res;
        res.resize(N);
        switch (b)
        {
          case bounding_mode::CLIP:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp(val[i], min[i], max[i]);
            break;
          case bounding_mode::WRAP:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::wrap(val[i], min[i], max[i]);
            break;
          case bounding_mode::FOLD:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::fold(val[i], min[i], max[i]);
            break;
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp_min(val[i], min[i]);
            break;
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp_max(val[i], max[i]);
            break;
          default:
            break;
        }
        return res;
      }
    }
    else if (has_min)
    {
      const std::vector<ossia::value>& min = *domain.min;
      if(min.size() == 1)
      {
        std::vector<ossia::value> res;
        res.resize(N);
        const auto min_v = min[0];
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp_min(val[i], min_v);
          default:
            break;
        }
        return res;
      }
      else if(N != min.size())
      {
        return {};
      }
      else
      {
        std::vector<ossia::value> res;
        res.resize(N);
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp_min(val[i], min[i]);
          default:
            break;
        }
        return res;
      }
    }
    else if (has_max)
    {
      const std::vector<ossia::value>& max = *domain.max;
      if(max.size() == 1)
      {
        std::vector<ossia::value> res;
        res.resize(N);
        const auto max_v = max[0];
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp_max(val[i], max_v);
          default:
            break;
        }
        return res;
      }
      else if(N != max.size())
      {
        return {};
      }
      else
      {
        std::vector<ossia::value> res;
        res.resize(N);
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) res[i] = ossia::clamp_max(val[i], max[i]);
          default:
            break;
        }
        return res;
      }
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

value tuple_clamp::operator()(bounding_mode b, std::vector<ossia::value>&& val) const
{
  if (b == bounding_mode::FREE)
    return val;

  // We handle values by checking component by component
  const auto& values = domain.values;
  if(values.empty())
  {
    const auto N = val.size();
    const bool has_min = bool(domain.min);
    const bool has_max = bool(domain.max);
    if (has_min && has_max)
    {
      const std::vector<ossia::value>& min = *domain.min;
      const std::vector<ossia::value>& max = *domain.max;

      if(min.size() == 1 && max.size() == 1)
      {
        const auto min_v = min[0];
        const auto max_v = max[0];

        switch (b)
        {
          case bounding_mode::CLIP:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp(val[i], min_v, max_v);
            break;
          case bounding_mode::WRAP:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::wrap(val[i], min_v, max_v);
            break;
          case bounding_mode::FOLD:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::fold(val[i], min_v, max_v);
            break;
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_min(val[i], min_v);
            break;
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_max(val[i], max_v);
            break;
          default:
            break;
        }
        return std::move(val);
      }
      else if(N != min.size() || N != max.size())
      {
        return {};
      }
      else
      {
        switch (b)
        {
          case bounding_mode::CLIP:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp(val[i], min[i], max[i]);
            break;
          case bounding_mode::WRAP:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::wrap(val[i], min[i], max[i]);
            break;
          case bounding_mode::FOLD:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::fold(val[i], min[i], max[i]);
            break;
          case bounding_mode::LOW:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_min(val[i], min[i]);
            break;
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_max(val[i], max[i]);
            break;
          default:
            break;
        }
        return std::move(val);
      }
    }
    else if (has_min)
    {
      const std::vector<ossia::value>& min = *domain.min;
      const auto min_v = min[0];

      if(min.size() == 1)
      {
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_min(val[i], min_v);
          default:
            break;
        }
        return std::move(val);
      }
      else if(N != min.size())
      {
        return {};
      }
      else
      {
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_min(val[i], min[i]);
          default:
            break;
        }
        return std::move(val);
      }
    }
    else if (has_max)
    {
      const std::vector<ossia::value>& max = *domain.max;
      const auto max_v = max[0];

      if(max.size() == 1)
      {
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_max(val[i], max_v);
          default:
            break;
        }
        return std::move(val);
      }
      else if(N != max.size())
      {
        return {};
      }
      else
      {
        switch(b)
        {
          case bounding_mode::CLIP:
          case bounding_mode::HIGH:
            for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_max(val[i], max[i]);
          default:
            break;
        }
        return std::move(val);
      }
    }
    return std::move(val);
  }
  else
  {
    auto it = values.find(val);
    return (it != values.end())
        ? std::move(val)
        : ossia::value{};
  }
}

template<std::size_t N>
value vec_clamp<N>::operator()(bounding_mode b, std::array<float, N> val) const
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
      const std::array<float, N>& min = *domain.min;
      const std::array<float, N>& max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp(val[i], min[i], max[i]);
          break;
        case bounding_mode::WRAP:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::wrap(val[i], min[i], max[i]);
          break;
        case bounding_mode::FOLD:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::fold(val[i], min[i], max[i]);
          break;
        case bounding_mode::LOW:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_min(val[i], min[i]);
          break;
        case bounding_mode::HIGH:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_max(val[i], max[i]);
          break;
        default:
          break;
      }
    }
    else if (has_min)
    {
      const std::array<float, N>& min = *domain.min;
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_min(val[i], min[i]);
        default:
          break;
      }
    }
    else if (has_max)
    {
      const std::array<float, N>& max = *domain.max;
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
          for(std::size_t i = 0; i < N; i++) val[i] = ossia::clamp_max(val[i], max[i]);
        default:
          break;
      }
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

value generic_clamp::operator()(bounding_mode b, const value& v) const
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
      const auto& min = *domain.min;
      const auto& max = *domain.max;
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
      const auto& min = *domain.min;
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
      const auto& max = *domain.max;
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


value generic_clamp::operator()(bounding_mode b, value&& v) const
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
      const auto& min = *domain.min;
      const auto& max = *domain.max;
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
      const auto& min = *domain.min;
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
      const auto& max = *domain.max;
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


value generic_clamp::operator()(bounding_mode b, const std::vector<ossia::value>& val) const
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
      std::vector<ossia::value> res;
      res.reserve(val.size());
      const auto& min = *domain.min;
      const auto& max = *domain.max;

      switch (b)
      {
        case bounding_mode::CLIP:
          for(auto& v : val) res.push_back(ossia::clamp(v, min, max));
          break;
        case bounding_mode::WRAP:
          for(auto& v : val) res.push_back(ossia::wrap(v, min, max));
          break;
        case bounding_mode::FOLD:
          for(auto& v : val) res.push_back(ossia::fold(v, min, max));
          break;
        case bounding_mode::LOW:
          for(auto& v : val) res.push_back(ossia::clamp_min(v, min));
          break;
        case bounding_mode::HIGH:
          for(auto& v : val) res.push_back(ossia::clamp_max(v, max));
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
          const auto& min = *domain.min;
          std::vector<ossia::value> res;
          res.reserve(val.size());
          for(auto& v : val)
          {
            res.push_back(ossia::clamp_min(v, min));
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
          const auto& max = *domain.max;
          std::vector<ossia::value> res;
          res.reserve(val.size());
          for(auto& v : val)
          {
            res.push_back(ossia::clamp_max(v, max));
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
    for(auto& v : val)
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


value generic_clamp::operator()(bounding_mode b, std::vector<ossia::value>&& val) const
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
      const auto& min = *domain.min;
      const auto& max = *domain.max;
      switch (b)
      {
        case bounding_mode::CLIP:
          for(auto& v : val) v = ossia::clamp(v, min, max);
          break;
        case bounding_mode::WRAP:
          for(auto& v : val) v = ossia::wrap(v, min, max);
          break;
        case bounding_mode::FOLD:
          for(auto& v : val) v = ossia::fold(v, min, max);
          break;
        case bounding_mode::LOW:
          for(auto& v : val) v = ossia::clamp_min(v, min);
          break;
        case bounding_mode::HIGH:
          for(auto& v : val) v = ossia::clamp_max(v, max);
          break;
        default:
          return std::move(val);
      }

      return std::move(val);
    }
    else if (has_min)
    {
      const auto& min = *domain.min;
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::LOW:
        {
          for(auto& v : val) v = ossia::clamp_min(v, min);
          return std::move(val);
        }
        default:
          return std::move(val);
      }
    }
    else if (has_max)
    {
      const auto& max = *domain.max;
      switch(b)
      {
        case bounding_mode::CLIP:
        case bounding_mode::HIGH:
        {
          for(auto& v : val) v = ossia::clamp_max(v, max);
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
    for(auto& v : val)
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



value apply_domain_visitor::operator()(int32_t value, const domain_base<int32_t>& domain) const
{ return numeric_clamp<domain_base<int32_t>>{domain}(b, value); }

value apply_domain_visitor::operator()(float value, const domain_base<float>& domain) const
{ return numeric_clamp<domain_base<float>>{domain}(b, value); }

value apply_domain_visitor::operator()(char value, const domain_base<char>& domain) const
{ return numeric_clamp<domain_base<char>>{domain}(b, value); }

value apply_domain_visitor::operator()(bool value, const domain_base<bool>& domain) const
{ return numeric_clamp<domain_base<bool>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::vector<ossia::value>& value, const domain_base<ossia::value>& domain) const
{
  std::vector<ossia::value> res = value;
  for(auto& val : res)
  {
    val = generic_clamp{domain}(b, val);
  }
  return res;
}

ossia::value apply_domain_visitor::operator()(std::vector<ossia::value>&& value, const domain_base<ossia::value>& domain) const
{
  for(auto& val : value)
  {
    val = generic_clamp{domain}(b, std::move(val));
  }
  // TODO currently other values (strings, etc...) are ignored; what should we do here ?
  return std::move(value);
}

// Second case : we filter a whole tuple.
ossia::value apply_domain_visitor::operator()(const std::vector<ossia::value>& value, const domain_base<std::vector<ossia::value>>& domain) const
{ return tuple_clamp{domain}(b, value); }
ossia::value apply_domain_visitor::operator()(std::vector<ossia::value>&& value, const domain_base<std::vector<ossia::value>>& domain) const
{ return tuple_clamp{domain}(b, std::move(value)); }

// Vec : we can either filter each value, or filter the whole shebang
ossia::value apply_domain_visitor::operator()(const std::array<float, 2>& value, const domain_base<float>& domain) const
{ return numeric_clamp<domain_base<float>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 2>& value, const domain_base<int32_t>& domain) const
{ return numeric_clamp<domain_base<int32_t>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 2>& value, const domain_base<bool>& domain) const
{ return numeric_clamp<domain_base<bool>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 2>& value, const domain_base<char>& domain) const
{ return numeric_clamp<domain_base<char>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 2>& value, const domain_base<std::array<float, 2>>& domain) const
{ return vec_clamp<2>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 2>& value, const domain_base<std::vector<ossia::value>>& domain) const
{ return vec_clamp<2>{ossia::net::domain_conversion<domain_base<std::array<float, 2>>>{}.tuple_func(domain)}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 3>& value, const domain_base<float>& domain) const
{ return numeric_clamp<domain_base<float>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 3>& value, const domain_base<int32_t>& domain) const
{ return numeric_clamp<domain_base<int32_t>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 3>& value, const domain_base<bool>& domain) const
{ return numeric_clamp<domain_base<bool>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 3>& value, const domain_base<char>& domain) const
{ return numeric_clamp<domain_base<char>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 3>& value, const domain_base<std::array<float, 3>>& domain) const
{ return vec_clamp<3>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 3>& value, const domain_base<std::vector<ossia::value>>& domain) const
{ return vec_clamp<3>{ossia::net::domain_conversion<domain_base<std::array<float, 3>>>{}.tuple_func(domain)}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 4>& value, const domain_base<float>& domain) const
{ return numeric_clamp<domain_base<float>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 4>& value, const domain_base<int32_t>& domain) const
{ return numeric_clamp<domain_base<int32_t>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 4>& value, const domain_base<bool>& domain) const
{ return numeric_clamp<domain_base<bool>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 4>& value, const domain_base<char>& domain) const
{ return numeric_clamp<domain_base<char>>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 4>& value, const domain_base<std::array<float, 4>>& domain) const
{ return vec_clamp<4>{domain}(b, value); }

ossia::value apply_domain_visitor::operator()(const std::array<float, 4>& value, const domain_base<std::vector<ossia::value>>& domain) const
{ return vec_clamp<4>{ossia::net::domain_conversion<domain_base<std::array<float, 4>>>{}.tuple_func(domain)}(b, value); }

}
}

std::ostream& operator<<(std::ostream& s, const ossia::net::domain& d)
{
  // OPTIMIZEME
  s << d.to_pretty_string();
  return s;
}
