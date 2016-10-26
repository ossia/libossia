#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_conversion.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <boost/type_traits/function_traits.hpp>
#define BOOST_LEXICAL_CAST_ASSUME_C_LOCALE
#include <boost/lexical_cast.hpp>
namespace ossia
{

namespace detail
{
template<typename T>
struct array_size;
template<typename T, std::size_t N>
struct array_size<std::array<T, N>> :
    public std::integral_constant<std::size_t, N>
{ };

template<typename Target, typename = void>
struct value_converter
{

};

template<>
struct value_converter<ossia::Impulse>
{
  template<typename U>
  ossia::Impulse operator()(const U&) { return {}; }

  ossia::Impulse operator()() { return {}; }
};

template<typename T>
struct numeric_value_converter
{
  T operator()(Impulse) const { return T{}; }
  T operator()(Int v) { return v; }
  T operator()(Float v) { return v; }
  T operator()(Bool v) { return v; }
  T operator()(Char v) { return v; }
  T operator()() const { return T{}; }

  T operator()(const Destination&) const { return T{}; }
  T operator()(const String& v) const {
    try {
      return boost::lexical_cast<T>(v);
    } catch( ... ) {
      return T{};
    }
  }
  T operator()(const Vec2f& v) const { return v[0]; }
  T operator()(const Vec3f& v) const { return v[0]; }
  T operator()(const Vec4f& v) const { return v[0]; }

  T operator()(const Tuple& v) const
  { return !v.empty() ? convert<T>(v[0]) : T{}; }
};


template<>
struct value_converter<int> : public numeric_value_converter<int>
{
};
template<>
struct value_converter<float> : public numeric_value_converter<float>
{
};
template<>
struct value_converter<double> : public numeric_value_converter<double>
{
};
template<>
struct value_converter<bool> : public numeric_value_converter<bool>
{
};
template<>
struct value_converter<char> : public numeric_value_converter<char>
{
};

template<>
struct value_converter<std::string>
{
  using T = std::string;
  T operator()(Impulse) const { return T{}; }
  T operator()(Int v) { return std::to_string(v); }
  T operator()(Float v) { return std::to_string(v); }
  T operator()(Bool v) { return std::to_string(v); }
  T operator()(Char v) { return std::to_string(v); }
  T operator()(const String& v) const { return v; }

  T operator()(const Destination&) const { return T{}; }
  T operator()() const { return T{}; }

  template<std::size_t N>
  T operator()(Vec<float, N> v) const
  {
    std::string s = "[";
    s += std::to_string(v[0]);
    for(std::size_t i = 1; i < N; i++)
      s += ", " + std::to_string(v[i]);
    s += "]";
    return s;
  }

  T operator()(const Tuple& v) const
  {
    using namespace std::literals;
    std::string s = "["s;
    const auto n = v.size();
    if(n > 0)
    {
      s += convert<std::string>(v[0]);

      for(std::size_t i = 1; i < n; i++)
        s += ", "s + convert<std::string>(v[i]);
    }
    s += "]"s;
    return s;
  }
};

template<>
struct value_converter<std::vector<ossia::value>>
{
  template<typename U>
  std::vector<ossia::value> operator()(const U& u)
  {
    return {u};
  }

  template<std::size_t N>
  std::vector<ossia::value> operator()(const Vec<float, N>& u)
  {
    std::vector<ossia::value> v;
    for(std::size_t i = 0; i < N; i++)
    {
      v.push_back(Float{u[i]});
    }
    return v;
  }

  std::vector<ossia::value> operator()(const Tuple& t)
  { return t; }
  std::vector<ossia::value> operator()(Tuple&& t)
  { return std::move(t); }

  std::vector<ossia::value> operator()() { return {}; }
};

template<std::size_t N>
struct value_converter<std::array<float, N>>
{
  template<typename U>
  std::array<float, N> operator()(const U&)
  {
    return {};
  }

  std::array<float, N> operator()(Vec<float, N> v)
  {
    return v;
  }

  std::array<float, N> operator()(Float f)
  {
    std::array<float, N> a;
    a.fill(f);
    return a;
  }

  std::array<float, N> operator()(Int f)
  {
    std::array<float, N> a;
    a.fill(f);
    return a;
  }

  std::array<float, N> operator()(Char f)
  {
    std::array<float, N> a;
    a.fill(f);
    return a;
  }

  std::array<float, N> operator()(Bool f)
  {
    std::array<float, N> a;
    a.fill(f ? 1. : 0.);
    return a;
  }

  std::array<float, N> operator()(const Tuple& t)
  {
    return convert<std::array<float, N>>(t);
  }

  std::array<float, N> operator()() { return {}; }
};

template<>
struct value_converter<Destination>
{
  template<typename... U>
  Destination operator()(U&&...)
  {
    throw invalid_value_type_error("value_converter<Destination>: "
                                   "Don't try to convert from / to a Destination");
  }
};

}

template<typename T>
T convert(const ossia::value& val)
{
  return val.apply(detail::value_converter<T>{});
}

// Used to convert Tuple in Vec2f, Vec3f, Vec4f...
template<typename T>
T convert(const ossia::Tuple& val)
{
  // TODO should we have an error if the Tuple does not
  // have the correct number of arguments ? Or just silently fill
  // with zeros ?

  T res;
  const auto N = std::min(val.size(), detail::array_size<T>::value);
  for(std::size_t i = 0; i < N; i++)
  {
    res[i] = convert<float>(val[i]);
  }
  return res;
}


// MOVEME
template<typename Fun, typename... Args>
auto lift(ossia::val_type type, Fun f, Args&&... args)
{
  switch (type)
  {
    case val_type::IMPULSE:
      return f(ossia::value_trait<Impulse>{}, std::forward<Args>(args)...);
    case val_type::BOOL:
      return f(ossia::value_trait<Bool>{}, std::forward<Args>(args)...);
    case val_type::INT:
      return f(ossia::value_trait<Int>{}, std::forward<Args>(args)...);
    case val_type::FLOAT:
      return f(ossia::value_trait<Float>{}, std::forward<Args>(args)...);
    case val_type::CHAR:
      return f(ossia::value_trait<Char>{}, std::forward<Args>(args)...);
    case val_type::STRING:
      return f(ossia::value_trait<String>{}, std::forward<Args>(args)...);
    case val_type::TUPLE:
      return f(ossia::value_trait<Tuple>{}, std::forward<Args>(args)...);
    case val_type::VEC2F:
      return f(ossia::value_trait<Vec2f>{}, std::forward<Args>(args)...);
    case val_type::VEC3F:
      return f(ossia::value_trait<Vec3f>{}, std::forward<Args>(args)...);
    case val_type::VEC4F:
      return f(ossia::value_trait<Vec4f>{}, std::forward<Args>(args)...);
    case val_type::DESTINATION:
      return f(ossia::value_trait<Destination>{}, std::forward<Args>(args)...);
  }

  throw invalid_value_type_error("lift: Invalid type");
  return decltype(f(ossia::value_trait<Impulse>{}, std::forward<Args>(args)...)){};
}

}
