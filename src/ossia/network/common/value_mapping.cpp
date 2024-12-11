#include <ossia/network/common/value_mapping.hpp>

namespace ossia
{
template <typename T, typename U, typename V>
struct scale_visitor
{
  T src_min{};
  U dst_min{};
  V ratio{};

  void operator()(ossia::impulse) const noexcept { }

  template <typename Val>
  void operator()(Val& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }

  void operator()(std::string& str) const noexcept
  {
    for(char& v : str)
      v = dst_min + ratio * (v - src_min);
  }

  template <std::size_t N>
  void operator()(std::array<float, N>& v) const noexcept
  {
    for(std::size_t i = 0; i < N; i++)
    {
      v[i] = dst_min + ratio * (v[i] - src_min);
    }
  }

  void operator()(std::vector<ossia::value>& v) const noexcept
  {
    for(auto& value : v)
      value.apply(*this);
  }

  void operator()(value_map_type& v) const noexcept
  {
    for(auto& [k, value] : v)
      value.apply(*this);
  }

  void operator()() const noexcept { }
};

template <typename T>
void scale(T& value, double src_min, double src_max, double dst_min, double dst_max)
{
  const double sub = src_max - src_min;
  if(sub == 0)
    return;

  auto ratio = (dst_max - dst_min) / sub;

  scale_visitor<double, double, double>{src_min, dst_min, ratio}(value);
}

template <typename T, typename U>
auto constrain(T& value, const std::vector<U>& acceptable)
{
}

struct domain_map
{
  const ossia::destination_index& index;

  template <typename Value_T, typename Domain_T1, typename Domain_T2>
  void
  operator()(Value_T& v, Domain_T1& source_domain, Domain_T2& tgt_domain) const noexcept
  {
  }

  // float value, vec3 domain, an index corresponding to the domain "index"
  template <std::size_t N, typename T>
  void operator()(
      T& v, ossia::vecf_domain<N>& source_domain,
      ossia::domain_base<T>& tgt_domain) const noexcept
  {
#if !defined(OSSIA_FREESTANDING)
    // FIXME
    if constexpr(std::is_same_v<int, T> || std::is_same_v<float, T>)
    {
      if(index.size() == 1 && index[0] < int(N))
      {
        const auto& smin = source_domain.min[index[0]];
        const auto& smax = source_domain.max[index[0]];
        if(smin && smax && tgt_domain.min && tgt_domain.max)
        {
          scale(v, *smin, *smax, *tgt_domain.min, *tgt_domain.max);
        }
      }

      if(!tgt_domain.values.empty())
      {
        constrain(v, tgt_domain.values);
      }
    }
#endif
  }

  template <typename Value_T>
  void operator()(
      Value_T& v, ossia::domain_base<int32_t>& source_domain,
      ossia::domain_base<int32_t>& tgt_domain) const noexcept
  {
    if(source_domain.min && source_domain.max && tgt_domain.min && tgt_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, *tgt_domain.min, *tgt_domain.max);
    }

    if(!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template <typename Value_T>
  void operator()(
      Value_T& v, ossia::domain_base<float>& source_domain,
      ossia::domain_base<float>& tgt_domain) const noexcept
  {
    if(source_domain.min && source_domain.max && tgt_domain.min && tgt_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, *tgt_domain.min, *tgt_domain.max);
    }

    if(!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template <typename Value_T>
  void operator()(
      Value_T& v, ossia::domain_base<int32_t>& source_domain,
      ossia::domain_base<float>& tgt_domain) const noexcept
  {
    if(source_domain.min && source_domain.max && tgt_domain.min && tgt_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, *tgt_domain.min, *tgt_domain.max);
    }

    if(!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template <typename Value_T>
  void operator()(
      Value_T& v, ossia::domain_base<float>& source_domain,
      ossia::domain_base<int32_t>& tgt_domain) const noexcept
  {
    if(source_domain.min && source_domain.max && tgt_domain.min && tgt_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, *tgt_domain.min, *tgt_domain.max);
    }

    if(!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template <typename Value_T>
  void operator()(
      Value_T& v, ossia::domain_base<int32_t>& source_domain,
      ossia::domain_base<bool>& tgt_domain) const noexcept
  {
    if(source_domain.min && source_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, 0, 1);
    }
  }

  template <typename Value_T>
  void operator()(
      Value_T& v, ossia::domain_base<float>& source_domain,
      ossia::domain_base<bool>& tgt_domain) const noexcept
  {
    if(source_domain.min && source_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, 0, 1);
    }
  }

  template <typename Value_T>
  void operator()(
      Value_T& v, ossia::domain_base<bool>& source_domain,
      ossia::domain_base<int32_t>& tgt_domain) const noexcept
  {
    if(tgt_domain.min && tgt_domain.max)
    {
      scale(v, 0, 1, *tgt_domain.min, *tgt_domain.max);
    }

    if(!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template <typename Value_T>
  void operator()(
      Value_T& v, ossia::domain_base<bool>& source_domain,
      ossia::domain_base<float>& tgt_domain) const noexcept
  {
    if(tgt_domain.min && tgt_domain.max)
    {
      scale(v, 0, 1, *tgt_domain.min, *tgt_domain.max);
    }

    if(!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }
};

void map_value(
    value& source, const ossia::destination_index& idx, const domain& source_domain,
    const domain& target_domain)
{
  ossia::apply(
      [&](auto& source_domain, auto& v) {
    using val_t = std::remove_const_t<std::remove_reference_t<decltype(v)>>;
    ossia::apply_nonnull(
        [&](auto& tgt_domain) {
      domain_map{idx}(const_cast<val_t&>(v), source_domain, tgt_domain);
        },
        const_cast<domain&>(target_domain).v);
      },
      const_cast<domain&>(source_domain).v, source.v);
}

}
