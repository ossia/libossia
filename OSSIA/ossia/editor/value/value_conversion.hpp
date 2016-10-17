#pragma once
#include <ossia/editor/value/value.hpp>
#include <ossia/editor/value/value_traits.hpp>
#include <boost/type_traits/function_traits.hpp>
namespace ossia
{


template<typename Target, typename = void>
struct value_converter
{

};

template<typename T>
T convert(const ossia::value& val);


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
  T operator()(Int v) { return v.value; }
  T operator()(Float v) { return v.value; }
  T operator()(Bool v) { return v.value; }
  T operator()(Char v) { return v.value; }
  T operator()() const { return T{}; }

  T operator()(const Behavior&) const { return T{}; }
  T operator()(const Destination&) const { return T{}; }
  T operator()(const String& v) const { return T{}; }
  T operator()(Vec2f v) const { return v.value[0]; }
  T operator()(Vec3f v) const { return v.value[0]; }
  T operator()(Vec4f v) const { return v.value[0]; }

  T operator()(const Tuple& v) const
  { return !v.value.empty() ? convert<T>(v.value[0]) : T{}; }
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
    T operator()(Int v) { return std::to_string(v.value); }
    T operator()(Float v) { return std::to_string(v.value); }
    T operator()(Bool v) { return std::to_string(v.value); }
    T operator()(Char v) { return std::to_string(v.value); }
    T operator()() const { return T{}; }

    T operator()(const Behavior&) const { return T{}; }
    T operator()(const Destination&) const { return T{}; }
    T operator()(const String& v) const { return T{}; }

    template<std::size_t N>
    T operator()(Vec<float, N> v) const
    {
      std::string s = "[";
      s += std::to_string(v.value[0]);
      for(std::size_t i = 1; i < N; i++)
        s += ", " + std::to_string(v.value[i]);
      s += "]";
      return s;
    }

    T operator()(const Tuple& v) const
    {
      using namespace std::literals;
      std::string s = "["s;
      const auto n = v.value.size();
      if(n > 0)
      {
        s += convert<std::string>(v.value[0]);

        for(std::size_t i = 1; i < n; i++)
          s += ", "s + convert<std::string>(v.value[i]);
      }
      s += "]"s;
      return s;
    }
  };

  template<>
  struct value_converter<std::vector<ossia::value>>
  {
    template<typename U>
    std::vector<ossia::value> operator()(const U&)
    {
      // TODO
      return {};
    }

    std::vector<ossia::value> operator()() { return {}; }
  };

  template<std::size_t N>
  struct value_converter<std::array<float, N>>
  {
    template<typename U>
    std::array<float, N> operator()(const U&)
    {
      // TODO
      return {};
    }

    std::array<float, N> operator()() { return {}; }
  };

  template<>
  struct value_converter<decltype(Behavior::value)>
  {
    using T = decltype(Behavior::value);
    template<typename U>
    T operator()(const U&)
    {
      return {};
    }

    T operator()() { return {}; }
  };

  template<>
  struct value_converter<Behavior>
  {
    template<typename... U>
    Behavior operator()(U&&...)
    {
      throw invalid_value_type_error("value_converter<Behavior>: "
                                     "Don't try to convert from / to a Behavior");
    }
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

  template<typename T>
  struct value_converter<T, ossia::void_t<typename value_trait<T>::ossia_type>>
  {
    T operator()() { return T{}; }

    template<typename U>
    T operator()(U&& u)
    {
      return T{value_converter<decltype(std::declval<T>().value)>{}(std::forward<U>(u))};
    }
  };

  template<typename T>
  T convert(const ossia::value& val)
  {
    return val.apply(value_converter<T>{});
  }

  // Used to convert Tuple in Vec2f, Vec3f, Vec4f...
  template<typename T>
  T convert(const ossia::Tuple& val)
  {
    // TODO should we have an error if the Tuple does not
    // have the correct number of arguments ? Or just silently fill
    // with zeros ?

    T res;
    int N = std::min((int)val.value.size(), (int)T::size_value);
    for(int i = 0; i < N; i++)
    {
      res.value[i] = convert<float>(val.value[i]);
    }
    return res;
  }

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
      case val_type::BEHAVIOR:
        return f(ossia::value_trait<Behavior>{}, std::forward<Args>(args)...);
      case val_type::DESTINATION:
        return f(ossia::value_trait<Destination>{}, std::forward<Args>(args)...);
    }

    throw invalid_value_type_error("lift: Invalid type");
    return decltype(f(ossia::value_trait<Impulse>{}, std::forward<Args>(args)...)){};
  }

  inline ossia::value convert(const ossia::value& val, ossia::val_type newtype)
  {
    return lift(newtype, [&] (auto t) -> ossia::value {
      using ossia_type = typename decltype(t)::ossia_type;
      return convert<ossia_type>(val);
    });
  }

}
