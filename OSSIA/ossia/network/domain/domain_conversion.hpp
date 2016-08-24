#pragma once
#include <ossia/network/domain/domain.hpp>

namespace ossia
{
namespace net
{
template <typename U>
struct domain_conversion
{

  template <typename T>
  domain operator()(const T&)
  {
    return U{};
  }

  domain operator()(const domain_base<Int>& t)
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

  domain operator()(const domain_base<Float>& t)
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

  domain operator()(const domain_base<Bool>& t)
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

  domain operator()(const domain_base<Char>& t)
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
struct domain_conversion<domain_base<Impulse>>
{
  template <typename T>
  domain operator()(const T&)
  {
    return domain_base<Impulse>{};
  }
};

template <>
struct domain_conversion<domain_base<Destination>>
{
  domain operator()(const domain_base<Destination>& src)
  {
    return src;
  }

  template <typename T>
  domain operator()(const T&)
  {
    return domain_base<Destination>{};
  }
};

template <>
struct domain_conversion<domain_base<Behavior>>
{
  domain operator()(const domain_base<Behavior>& src)
  {
    return src;
  }

  template <typename T>
  domain operator()(const T&)
  {
    return domain_base<Behavior>{};
  }
};

template <>
struct domain_conversion<domain_base<Tuple>>
{
  domain operator()(const domain_base<Tuple>& src)
  {
    return src;
  }

  template <typename T>
  domain operator()(const T&)
  {
    return domain_base<Tuple>{};
  }
};

template <int N>
struct domain_conversion<domain_base<Vec<float, N>>>
{
  domain operator()(const domain_base<Vec<float, N>>& src)
  {
    return src;
  }

  template <typename T>
  domain operator()(const T&)
  {
    return domain_base<Vec<float, N>>{};
  }
};

template <>
struct domain_conversion<domain_base<String>>
{
  domain operator()(const domain_base<String>& src)
  {
    return src;
  }

  template <typename T>
  domain operator()(const T&)
  {
    return domain_base<String>();
  }
};

inline domain convert_domain(const domain& dom, ossia::val_type newtype)
{
  switch (newtype)
  {
    case val_type::IMPULSE:
      return eggs::variants::apply(
          domain_conversion<domain_base<Impulse>>{}, dom);
    case val_type::INT:
      return eggs::variants::apply(
          domain_conversion<domain_base<Int>>{}, dom);
    case val_type::FLOAT:
      return eggs::variants::apply(
          domain_conversion<domain_base<Float>>{}, dom);
    case val_type::BOOL:
      return eggs::variants::apply(
          domain_conversion<domain_base<Bool>>{}, dom);
    case val_type::CHAR:
      return eggs::variants::apply(
          domain_conversion<domain_base<Char>>{}, dom);
    case val_type::STRING:
      return eggs::variants::apply(
          domain_conversion<domain_base<String>>{}, dom);
    case val_type::TUPLE:
      return eggs::variants::apply(
          domain_conversion<domain_base<Tuple>>{}, dom);
    case val_type::VEC2F:
      return eggs::variants::apply(
          domain_conversion<domain_base<Vec2f>>{}, dom);
    case val_type::VEC3F:
      return eggs::variants::apply(
          domain_conversion<domain_base<Vec3f>>{}, dom);
    case val_type::VEC4F:
      return eggs::variants::apply(
          domain_conversion<domain_base<Vec4f>>{}, dom);
    case val_type::DESTINATION:
      return eggs::variants::apply(
          domain_conversion<domain_base<Destination>>{}, dom);
    case val_type::BEHAVIOR:
      return eggs::variants::apply(
          domain_conversion<domain_base<Behavior>>{}, dom);
    default:
      return domain{};
  }
}

}
}
