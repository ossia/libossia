#include <ossia/network/common/value_mapping.hpp>

namespace ossia
{
template<typename T, typename U, typename V>
struct scale_visitor
{
  T src_min{};
  U dst_min{};
  V ratio{};

  void operator()(ossia::impulse) const noexcept
  {

  }

  void operator()(int& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }
  void operator()(float& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }
  void operator()(bool& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }

  void operator()(char& v) const noexcept
  {
    v = dst_min + ratio * (v - src_min);
  }
  void operator()(std::string& str) const noexcept
  {
    for(char& v : str)
      v = dst_min + ratio * (v - src_min);
  }

  void operator()(ossia::vec2f& v) const noexcept
  {
    v[0] = dst_min + ratio * (v[0] - src_min);
    v[1] = dst_min + ratio * (v[1] - src_min);
  }
  void operator()(ossia::vec3f& v) const noexcept
  {
    v[0] = dst_min + ratio * (v[0] - src_min);
    v[1] = dst_min + ratio * (v[1] - src_min);
    v[2] = dst_min + ratio * (v[2] - src_min);
  }
  void operator()(ossia::vec4f& v) const noexcept
  {
    v[0] = dst_min + ratio * (v[0] - src_min);
    v[1] = dst_min + ratio * (v[1] - src_min);
    v[2] = dst_min + ratio * (v[2] - src_min);
    v[3] = dst_min + ratio * (v[3] - src_min);
  }

  void operator()(std::vector<ossia::value>& v) const noexcept
  {
    for(auto& value : v)
      value.apply(*this);
  }

  void operator()() const noexcept
  {
  }
};

template<typename T, typename U, typename V>
void scale(T& value, U src_min, U src_max, V dst_min, V dst_max)
{
  const U sub = src_max - src_min;
  if(sub == 0)
    return;

  auto ratio = (dst_max - dst_min) / sub;

  scale_visitor<U, V, decltype(ratio)>{src_min, dst_min, ratio}(value);
}

template<typename T, typename U>
auto constrain(T& value, const std::vector<U>& acceptable)
{

}

struct domain_map
{
  template<typename Value_T, typename Domain_T1, typename Domain_T2>
  void operator()(Value_T& v, Domain_T1& source_domain, Domain_T2& tgt_domain) const noexcept
  {
  }

  template<typename Value_T>
  void operator()(Value_T& v, ossia::domain_base<int>& source_domain, ossia::domain_base<int>& tgt_domain) const noexcept
  {
    if (source_domain.min && source_domain.max && tgt_domain.min && tgt_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, *tgt_domain.min, *tgt_domain.max);
    }

    if (!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template<typename Value_T>
  void operator()(Value_T& v, ossia::domain_base<float>& source_domain, ossia::domain_base<float>& tgt_domain) const noexcept
  {
    if (source_domain.min && source_domain.max && tgt_domain.min && tgt_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, *tgt_domain.min, *tgt_domain.max);
    }

    if (!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template<typename Value_T>
  void operator()(Value_T& v, ossia::domain_base<int>& source_domain, ossia::domain_base<float>& tgt_domain) const noexcept
  {
    if (source_domain.min && source_domain.max && tgt_domain.min && tgt_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, *tgt_domain.min, *tgt_domain.max);
    }

    if (!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template<typename Value_T>
  void operator()(Value_T& v, ossia::domain_base<float>& source_domain, ossia::domain_base<int>& tgt_domain) const noexcept
  {
    if (source_domain.min && source_domain.max && tgt_domain.min && tgt_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, *tgt_domain.min, *tgt_domain.max);
    }

    if (!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template<typename Value_T>
  void operator()(Value_T& v, ossia::domain_base<int>& source_domain, ossia::domain_base<bool>& tgt_domain) const noexcept
  {
    if (source_domain.min && source_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, 0, 1);
    }
  }

  template<typename Value_T>
  void operator()(Value_T& v, ossia::domain_base<float>& source_domain, ossia::domain_base<bool>& tgt_domain) const noexcept
  {
    if (source_domain.min && source_domain.max)
    {
      scale(v, *source_domain.min, *source_domain.max, 0, 1);
    }
  }

  template<typename Value_T>
  void operator()(Value_T& v, ossia::domain_base<bool>& source_domain, ossia::domain_base<int>& tgt_domain) const noexcept
  {
    if (tgt_domain.min && tgt_domain.max)
    {
      scale(v, 0, 1, *tgt_domain.min, *tgt_domain.max);
    }

    if (!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }

  template<typename Value_T>
  void operator()(Value_T& v, ossia::domain_base<bool>& source_domain, ossia::domain_base<float>& tgt_domain) const noexcept
  {
    if (tgt_domain.min && tgt_domain.max)
    {
      scale(v, 0, 1, *tgt_domain.min, *tgt_domain.max);
    }

    if (!tgt_domain.values.empty())
    {
      constrain(v, tgt_domain.values);
    }
  }
};

void map_value(value& source, const domain& source_domain, const domain& target_domain)
{
  ossia::apply([&] (auto& source_domain, auto& v) {
    using val_t = std::remove_const_t<std::remove_reference_t<decltype(v)>>;
    ossia::apply_nonnull([&] (auto& tgt_domain) {
      domain_map{}(const_cast<val_t&>(v), source_domain, tgt_domain);
    }, const_cast<domain&>(target_domain).v);
  }, const_cast<domain&>(source_domain).v, source.v);
}

}
