#pragma once
#include <ossia/network/domain/domain.hpp>

namespace ossia
{
template <typename U>
struct domain_conversion
{

  template <typename T>
  OSSIA_INLINE domain operator()(const T&)
  {
    return U{};
  }

  domain operator()(const domain_base<int32_t>& t)
  {
    U f;
    if (t.min)
      f.min = *t.min;
    if (t.max)
      f.max = *t.max;
    if (!t.values.empty())
      for (auto val : t.values)
        f.values.insert(val);
    return f;
  }

  domain operator()(const domain_base<float>& t)
  {
    U f;
    if (t.min)
      f.min = *t.min;
    if (t.max)
      f.max = *t.max;
    if (!t.values.empty())
      for (auto val : t.values)
        f.values.insert(val);
    return f;
  }

  domain operator()(const domain_base<bool>& t)
  {
    U f;
    if (t.min)
      f.min = *t.min;
    if (t.max)
      f.max = *t.max;
    if (!t.values.empty())
      for (auto val : t.values)
        f.values.insert(val);
    return f;
  }

  domain operator()(const domain_base<char>& t)
  {
    U f;
    if (t.min)
      f.min = *t.min;
    if (t.max)
      f.max = *t.max;
    if (!t.values.empty())
      for (auto val : t.values)
        f.values.insert(val);
    return f;
  }
};

template <>
struct domain_conversion<domain_base<impulse>>
{
  template <typename T>
  OSSIA_INLINE domain operator()(const T&)
  {
    return domain_base<impulse>{};
  }
};

template <>
struct domain_conversion<vector_domain>
{
  OSSIA_INLINE domain operator()(const vector_domain& src)
  {
    return src;
  }

  template <typename T>
  OSSIA_INLINE domain operator()(const T&)
  {
    return vector_domain();
  }
};

template <std::size_t N>
struct domain_conversion<vecf_domain<N>>
{
  OSSIA_INLINE domain operator()(const vecf_domain<N>& src)
  {
    return src;
  }
  vecf_domain<N> tuple_func(const vector_domain& t)
  {
      vecf_domain<N> dom;
      const std::size_t min_size = std::min(N, t.min.size());
      for(std::size_t i = 0; i < min_size; i++)
      {
        dom.min[i] = ossia::convert<float>(t.min[i]);
      }

      const std::size_t max_size = std::min(N, t.max.size());
      for(std::size_t i = 0; i < max_size; i++)
      {
        dom.max[i] = ossia::convert<float>(t.max[i]);
      }


      const std::size_t vals_size = std::min(N, t.values.size());
      for(std::size_t i = 0; i < vals_size; i++)
      {
        dom.values[i].clear();
        for(auto& val : t.values[i])
          dom.values[i].insert(ossia::convert<float>(val));
      }

      return dom;
  }

  OSSIA_INLINE domain operator()(const vector_domain& t)
  {
      return tuple_func(t);
  }

  template <typename T>
  OSSIA_INLINE domain operator()(const T&)
  {
    return vecf_domain<N>();
  }
};

template <>
struct domain_conversion<domain_base<std::string>>
{
  OSSIA_INLINE domain operator()(const domain_base<std::string>& src)
  {
    return src;
  }

  template <typename T>
  OSSIA_INLINE domain operator()(const T&)
  {
    return domain_base<std::string>();
  }
};

// TODO handle the ossia::value case

inline domain convert_domain(const domain& dom, ossia::val_type newtype)
{
  switch (newtype)
  {
    case val_type::IMPULSE:
      return eggs::variants::apply(
          domain_conversion<domain_base<impulse>>{}, dom);
    case val_type::INT:
      return eggs::variants::apply(
          domain_conversion<domain_base<int32_t>>{}, dom);
    case val_type::FLOAT:
      return eggs::variants::apply(
          domain_conversion<domain_base<float>>{}, dom);
    case val_type::BOOL:
      return eggs::variants::apply(
          domain_conversion<domain_base<bool>>{}, dom);
    case val_type::CHAR:
      return eggs::variants::apply(
          domain_conversion<domain_base<char>>{}, dom);
    case val_type::STRING:
      return eggs::variants::apply(
          domain_conversion<domain_base<std::string>>{}, dom);
    case val_type::TUPLE:
      return eggs::variants::apply(
          domain_conversion<vector_domain>{}, dom);
    case val_type::VEC2F:
      return eggs::variants::apply(
          domain_conversion<vecf_domain<2>>{}, dom);
    case val_type::VEC3F:
      return eggs::variants::apply(
          domain_conversion<vecf_domain<3>>{}, dom);
    case val_type::VEC4F:
      return eggs::variants::apply(
          domain_conversion<vecf_domain<4>>{}, dom);
    case val_type::DESTINATION:
    default:
      return domain{};
  }
}

}
