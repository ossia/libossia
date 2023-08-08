#pragma once
#include <ossia/detail/config.hpp>

#include <ossia/detail/fmt.hpp>
#include <ossia/detail/parse_relax.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/value/value_conversion.hpp>
#include <ossia/network/value/value_traits.hpp>

#include <boost/type_traits/function_traits.hpp>
namespace ossia
{

namespace detail
{
template <typename T>
struct array_size;
template <typename T, std::size_t N>
struct array_size<std::array<T, N>> : public std::integral_constant<std::size_t, N>
{
};

template <typename Target, typename = void>
struct value_converter
{
};

template <>
struct value_converter<ossia::impulse>
{
  ossia::impulse cur;
  template <typename U>
  ossia::impulse operator()(const U&)
  {
    return {};
  }

  ossia::impulse operator()() { return {}; }
};

template <typename T>
struct numeric_value_converter
{
  T cur{};

  T operator()(impulse) const { return T{}; }
  T operator()(int32_t v) { return v; }
  T operator()(float v) { return v; }
  T operator()(bool v) { return v; }
  T operator()(char v) { return v; }
  T operator()() const { return T{}; }

  T operator()(const std::string& v) const = delete;
  T operator()(const vec2f& v) const { return v[0]; }
  T operator()(const vec3f& v) const { return v[0]; }
  T operator()(const vec4f& v) const { return v[0]; }

  T operator()(const std::vector<ossia::value>& v) const
  {
    return !v.empty() ? convert<T>(v[0]) : T{};
  }
  T operator()(const value_map_type& v) const
  {
    return !v.empty() ? convert<T>(v.begin()->second) : T{};
  }
};

template <>
struct value_converter<int32_t> : public numeric_value_converter<int32_t>
{
  using numeric_value_converter<int32_t>::operator();
  int32_t operator()(const std::string& v) const
  {
    if(auto n = parse_relax<int>(v))
      return *n;
    return {};
  }
};
template <>
struct value_converter<float> : public numeric_value_converter<float>
{
  using numeric_value_converter<float>::operator();
  float operator()(const std::string& v) const
  {
    if(auto n = parse_relax<float>(v))
      return *n;
    return {};
  }
};
template <>
struct value_converter<double> : public numeric_value_converter<double>
{
  using numeric_value_converter<double>::operator();
  double operator()(const std::string& v) const
  {
    if(auto n = parse_relax<double>(v))
      return *n;
    return {};
  }
};
template <>
struct value_converter<bool> : public numeric_value_converter<bool>
{
  using numeric_value_converter<bool>::operator();
  bool operator()(const std::string& v) const
  {
    return v.starts_with('T') || v.starts_with('t') || v.starts_with('Y')
           || v.starts_with('y') || v == "1";
  }
};
template <>
struct value_converter<char> : public numeric_value_converter<char>
{
};

#if defined(OSSIA_HAS_FMT)
struct fmt_writer
{
  fmt::memory_buffer& wr;

  void operator()(impulse) const { fmt::format_to(fmt::appender(wr), "impulse"); }
  void operator()(int32_t v) const { fmt::format_to(fmt::appender(wr), "{}", v); }
  void operator()(float v) const { fmt::format_to(fmt::appender(wr), "{}", v); }
  void operator()(bool v) const
  {
    if(v)
      fmt::format_to(fmt::appender(wr), "true");
    else
      fmt::format_to(fmt::appender(wr), "false");
  }
  void operator()(char v) const { fmt::format_to(fmt::appender(wr), "{}", v); }
  void operator()(const std::string& v) const
  {
    fmt::format_to(fmt::appender(wr), "{}", v);
  }
  void operator()() const { }
  template <std::size_t N>
  void operator()(std::array<float, N> v) const
  {
    fmt::format_to(fmt::appender(wr), "[{}", v[0]);
    for(std::size_t i = 1; i < N; i++)
      fmt::format_to(fmt::appender(wr), ", {}", v[i]);
    fmt::format_to(fmt::appender(wr), "]");
  }
  void operator()(const std::vector<ossia::value>& v) const
  {
    using namespace std::literals;
    fmt::format_to(fmt::appender(wr), "[");
    const auto n = v.size();
    if(n > 0)
    {
      v[0].apply(*this);

      for(std::size_t i = 1; i < n; i++)
      {
        fmt::format_to(fmt::appender(wr), ", ");
        v[i].apply(*this);
      }
    }
    fmt::format_to(fmt::appender(wr), "]");
  }
  void operator()(const value_map_type& v) const
  {
    using namespace std::literals;
    fmt::format_to(fmt::appender(wr), "{{");
    const auto n = v.size();
    if(n > 0)
    {
      auto it = v.begin();
      fmt::format_to(fmt::appender(wr), "\"{}\": ", it->first);
      it->second.apply(*this);

      for(++it; it != v.end(); ++it)
      {
        fmt::format_to(fmt::appender(wr), ", \"{}\": ", it->first);
        it->second.apply(*this);
      }
    }
    fmt::format_to(fmt::appender(wr), "}}");
  }
};

template <>
struct value_converter<std::string>
{
  const std::string& cur;
  using T = std::string;
  T operator()(impulse) const { return "impulse"; }
  T operator()(int32_t v) const { return fmt::format("{}", v); }
  T operator()(float v) const { return fmt::format("{}", v); }
  T operator()(bool v) const
  {
    using namespace std::literals;
    return v ? "true"s : "false"s;
  }
  T operator()(char v) const { return std::string(1, v); }
  T operator()(const std::string& v) const { return v; }

  T operator()() const { return {}; }

  template <std::size_t N>
  T operator()(std::array<float, N> v) const
  {
    std::string wr;
    wr.reserve(N * 10);
    fmt::format_to(std::back_inserter(wr), "[{}", v[0]);
    for(std::size_t i = 1; i < N; i++)
      fmt::format_to(std::back_inserter(wr), ", {}", v[i]);
    fmt::format_to(std::back_inserter(wr), "]");
    return wr;
  }

  T operator()(const std::vector<ossia::value>& v) const
  {
    using namespace std::literals;
    fmt::memory_buffer wr;
    fmt_writer{wr}(v);
    return {wr.data(), wr.size()};
  }

  T operator()(const value_map_type& v) const
  {
    using namespace std::literals;
    fmt::memory_buffer wr;
    fmt_writer{wr}(v);
    return {wr.data(), wr.size()};
  }
};
#else

template <>
struct value_converter<std::string>
{
  const std::string& cur;
  using T = std::string;
  T operator()(impulse) const { return "impulse"; }
  T operator()(int32_t v) const { return std::to_string(v); }
  T operator()(float v) const { return std::to_string(v); }
  T operator()(bool v) const
  {
    using namespace std::literals;
    return v ? "true"s : "false"s;
  }
  T operator()(char v) const { return std::string(1, v); }
  T operator()(const std::string& v) const { return v; }

  T operator()() const { return {}; }

  template <std::size_t N>
  T operator()(std::array<float, N> v) const
  {
    std::string wr;
    wr.reserve(N * 10);
    wr += "[";
    wr += std::to_string(v[0]);

    for(std::size_t i = 1; i < N; i++)
    {
      wr += ", ";
      wr += std::to_string(v[i]);
    }
    wr += "]";
    return wr;
  }

  T operator()(const std::vector<ossia::value>& v) const { return "(TODO)"; }

  T operator()(const value_map_type& v) const { return "(TODO)"; }
};
#endif

template <>
struct value_converter<std::vector<ossia::value>>
{
  const std::vector<ossia::value>& cur;
  template <typename U>
  std::vector<ossia::value> operator()(const U& u)
  {
    return {u};
  }

  template <std::size_t N>
  std::vector<ossia::value> operator()(const std::array<float, N>& u)
  {
    std::vector<ossia::value> v;
    for(std::size_t i = 0; i < N; i++)
    {
      v.emplace_back(float{u[i]});
    }
    return v;
  }

  std::vector<ossia::value> operator()(const std::vector<ossia::value>& t) { return t; }
  std::vector<ossia::value> operator()(std::vector<ossia::value>&& t)
  {
    return std::move(t);
  }

  std::vector<ossia::value> operator()() { return {}; }
};

template <>
struct value_converter<value_map_type>
{
  const value_map_type& cur;
  template <typename U>
  value_map_type operator()(const U& u)
  {
    return value_map_type{{"0", u}};
  }

  template <std::size_t N>
  value_map_type operator()(const std::array<float, N>& u)
  {
    value_map_type v;
    for(std::size_t i = 0; i < N; i++)
    {
      v[std::to_string(i)] = float{u[i]};
    }
    return v;
  }

  value_map_type operator()(const std::vector<ossia::value>& t)
  {
    value_map_type v;
    for(std::size_t i = 0; i < t.size(); i++)
    {
      v[std::to_string(i)] = t[i];
    }
    return v;
  }
  value_map_type operator()(std::vector<ossia::value>&& t)
  {
    value_map_type v;
    for(std::size_t i = 0; i < t.size(); i++)
    {
      v[std::to_string(i)] = std::move(t[i]);
    }
    return v;
  }

  value_map_type operator()(const value_map_type& t) { return t; }
  value_map_type operator()(value_map_type&& t) { return std::move(t); }

  value_map_type operator()() { return {}; }
};

template <std::size_t N>
struct value_converter<std::array<float, N>>
{
  const std::array<float, N>& cur;
  template <typename U>
  std::array<float, N> operator()(const U&)
  {
    return {};
  }

  std::array<float, N> operator()(std::array<float, N> v) { return v; }

  template <std::size_t M>
  std::array<float, N> operator()(std::array<float, M> v)
  {
    std::array<float, N> a = cur;
    for(std::size_t i = 0; i < std::min(N, M); i++)
    {
      a[i] = v[i];
    }
    return a;
  }

  std::array<float, N> operator()(float f)
  {
    std::array<float, N> a;
    a.fill(f);
    return a;
  }

  std::array<float, N> operator()(int32_t f)
  {
    std::array<float, N> a;
    a.fill(f);
    return a;
  }

  std::array<float, N> operator()(char f)
  {
    std::array<float, N> a;
    a.fill(f);
    return a;
  }

  std::array<float, N> operator()(bool f)
  {
    std::array<float, N> a;
    a.fill(f ? 1. : 0.);
    return a;
  }

  std::array<float, N> operator()(const std::vector<ossia::value>& t)
  {
    return convert<std::array<float, N>>(t);
  }

  std::array<float, N> operator()(const value_map_type& t) { return {}; }

  std::array<float, N> operator()() { return {}; }
};
}

template <typename T>
T convert(const ossia::value& val)
{
  return val.apply(detail::value_converter<T>{{T{}}});
}

template <typename T>
void convert_inplace(ossia::value& val)
{
  val = val.apply(detail::value_converter<T>{{T{}}});
}

template <typename T>
T convert(const T& cur, const ossia::value& val)
{
  return val.apply(detail::value_converter<T>{{cur}});
}

// Used to convert List in Vec2f, Vec3f, Vec4f...
template <typename T>
T convert(const std::vector<ossia::value>& val)
{
  // TODO should we have an error if the List does not
  // have the correct number of arguments ? Or just silently fill
  // with zeros ?

  T res{};
  const auto N = std::min(val.size(), detail::array_size<T>::value);
  for(std::size_t i = 0; i < N; i++)
  {
    res[i] = val[i].apply(detail::value_converter<typename T::value_type>{});
  }
  return res;
}

// MOVEME
template <typename Fun, typename... Args>
auto lift(ossia::val_type type, Fun f, Args&&... args)
{
  switch(type)
  {
    case val_type::IMPULSE:
      return f(ossia::value_trait<impulse>{}, std::forward<Args>(args)...);
    case val_type::BOOL:
      return f(ossia::value_trait<bool>{}, std::forward<Args>(args)...);
    case val_type::INT:
      return f(ossia::value_trait<int32_t>{}, std::forward<Args>(args)...);
    case val_type::FLOAT:
      return f(ossia::value_trait<float>{}, std::forward<Args>(args)...);
    case val_type::MAP:
      return f(ossia::value_trait<value_map_type>{}, std::forward<Args>(args)...);
    case val_type::STRING:
      return f(ossia::value_trait<std::string>{}, std::forward<Args>(args)...);
    case val_type::LIST:
      return f(
          ossia::value_trait<std::vector<ossia::value>>{}, std::forward<Args>(args)...);
    case val_type::VEC2F:
      return f(ossia::value_trait<vec2f>{}, std::forward<Args>(args)...);
    case val_type::VEC3F:
      return f(ossia::value_trait<vec3f>{}, std::forward<Args>(args)...);
    case val_type::VEC4F:
      return f(ossia::value_trait<vec4f>{}, std::forward<Args>(args)...);
    case val_type::NONE:
      break;
  }

  ossia_do_throw(invalid_value_type_error, "lift: Invalid type");
  return decltype(f(ossia::value_trait<impulse>{}, std::forward<Args>(args)...)){};
}
template <typename Fun, typename... Args>
auto lift_inplace(ossia::val_type type, Fun f, Args&&... args)
{
  switch(type)
  {
    case val_type::IMPULSE:
      f(ossia::value_trait<impulse>{}, std::forward<Args>(args)...);
      break;
    case val_type::BOOL:
      f(ossia::value_trait<bool>{}, std::forward<Args>(args)...);
      break;
    case val_type::INT:
      f(ossia::value_trait<int32_t>{}, std::forward<Args>(args)...);
      break;
    case val_type::FLOAT:
      f(ossia::value_trait<float>{}, std::forward<Args>(args)...);
      break;
    case val_type::MAP:
      f(ossia::value_trait<value_map_type>{}, std::forward<Args>(args)...);
      break;
    case val_type::STRING:
      f(ossia::value_trait<std::string>{}, std::forward<Args>(args)...);
      break;
    case val_type::LIST:
      f(ossia::value_trait<std::vector<ossia::value>>{}, std::forward<Args>(args)...);
      break;
    case val_type::VEC2F:
      f(ossia::value_trait<vec2f>{}, std::forward<Args>(args)...);
      break;
    case val_type::VEC3F:
      f(ossia::value_trait<vec3f>{}, std::forward<Args>(args)...);
      break;
    case val_type::VEC4F:
      f(ossia::value_trait<vec4f>{}, std::forward<Args>(args)...);
      break;
    case val_type::NONE:
      break;
  }
}
}

extern template double ossia::convert<double>(const ossia::value&);
extern template float ossia::convert<float>(const ossia::value&);
extern template int32_t ossia::convert<int32_t>(const ossia::value&);
// extern template char ossia::convert<char>(const ossia::value&);
extern template bool ossia::convert<bool>(const ossia::value&);
extern template std::string ossia::convert<std::string>(const ossia::value&);
extern template std::vector<ossia::value>
ossia::convert<std::vector<ossia::value>>(const ossia::value&);
extern template ossia::vec2f ossia::convert<ossia::vec2f>(const ossia::value&);
extern template ossia::vec3f ossia::convert<ossia::vec3f>(const ossia::value&);
extern template ossia::vec4f ossia::convert<ossia::vec4f>(const ossia::value&);
