#pragma once
#include <ossia/editor/value/value_base.hpp>
#include <ossia/editor/exceptions.hpp>

#include <ossia/detail/logger.hpp>
#include <eggs/variant.hpp>

#include <ossia_export.h>
namespace ossia
{
class value;

/**
 * \brief ossia::apply : helper function to apply a visitor to a variant
 * without throwing in the empty variant case.
 *
 * By default, a eggs::variant throws bad_variant.
 * In this case, the operator()() without arguments will be called.
 * This allows a simpler handling of the default case.
 */
template <typename Visitor, typename Variant>
auto apply(Visitor&& v, Variant&& var) -> decltype(auto)
{
  // Thanks K-Ballo (eggs-cpp/variant#21)
  if (var)
    return eggs::variants::apply(
        std::forward<Visitor>(v),
        std::forward<Variant>(var));
  else
    return std::forward<Visitor>(v)();
}


/*!
 * \brief getValueAsString Returns a string corresponding to the value
 * \param val a valid value
 * \return a string in the format : "type: value".
 *
 * ex. "int: 3"
 *     "string: tutu"
 *     "tuple: [ int: 2, float: 3 ]"
 * etc...
 *
 */
OSSIA_EXPORT std::string to_pretty_string(const ossia::value& val);

/**
 * @brief The value class
 *
 * Core type of the ossia API.
 * A value is a variant of multiple fundamental types.
 *
 * A value can be in an "unset" state; it has to be checked for this.
 * e.g. :
 * \code
 * void f(ossia::value& v) {
 *   if(v.valid())
 *   {
 *     auto maybe_int = v.try_get<Int>();
 *     if(maybe_int)
 *       std::cout << maybe_int->value;
 *     else // we know for some reason that it is a float
 *       std::cout << v.get<Float>().value;
 *   }
 * }
 * \endcode
 *
 * A generic operation can be applied safely to a value with a visitor.
 * See for instance \ref to_pretty_string.
 */
class OSSIA_EXPORT value
{
public:
  using value_type
      = eggs::variant<Impulse, Bool, Int, Float, Char, String, Tuple, Vec2f,
                      Vec3f, Vec4f, Destination, Behavior>;

  value_type v;

  // Construction
  template <typename T>
  value(T*) = delete;
  value(Impulse val) : v{val} { }
  value(Bool val) : v{val} { }
  value(Int val) : v{val} { }
  value(Float val) : v{val} { }
  value(Char val) : v{val} { }
  value(const String& val) : v{val} { }
  value(const Tuple& val) : v{val} { }
  value(const Vec2f& val) : v{val} { }
  value(const Vec3f& val) : v{val} { }
  value(const Vec4f& val) : v{val} { }
  value(const Destination& val) : v{val} { }
  value(const Behavior& val) : v{val} { }

  // Movable overloads
  value(String&& val) : v{std::move(val)} { }
  value(Tuple&& val) : v{std::move(val)} { }
  value(Destination&& val) : v{std::move(val)} { }
  value(Behavior&& val) : v{std::move(val)} { }


  // Assignment
  value& operator=(Impulse val)
  {
    v = val;
    return *this;
  }
  value& operator=(Bool val)
  {
    v = val;
    return *this;
  }
  value& operator=(Int val)
  {
    v = val;
    return *this;
  }
  value& operator=(Float val)
  {
    v = val;
    return *this;
  }
  value& operator=(Char val)
  {
    v = val;
    return *this;
  }
  value& operator=(const String& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const Tuple& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const Vec2f& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const Vec3f& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const Vec4f& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const Destination& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const Behavior& val)
  {
    v = val;
    return *this;
  }

  // Movable overloads
  value& operator=(String&& val)
  {
    v = std::move(val);
    return *this;
  }
  value& operator=(Tuple&& val)
  {
    v = std::move(val);
    return *this;
  }
  value& operator=(Destination&& val)
  {
    v = std::move(val);
    return *this;
  }
  value& operator=(Behavior&& val)
  {
    v = std::move(val);
    return *this;
  }

  value() = default;
  value(const value& other) = default;
  value(value&& other) = default;
  value& operator=(const value& other) = default;
  value& operator=(value&& other) = default;


  // Operations
  template <typename T>
  const T& get() const
  {
    return eggs::variants::get<T>(v);
  }

  template <typename T>
  T& get()
  {
    return eggs::variants::get<T>(v);
  }

  template <typename T>
  const T* try_get() const
  {
    return v.target<T>();
  }

  template <typename T>
  T* try_get()
  {
    return v.target<T>();
  }

  ossia::val_type getType() const
  {
    auto t = v.which();
    if (t == v.npos)
    {
      throw ossia::invalid_value_type_error{"value::getType: "
                                            "called with type == npos"};
      return {};
    }

    return static_cast<ossia::val_type>(t);
  }

  bool valid() const
  {
    return v.target();
  }

  void reset()
  {
    v = value_type{};
  }

  template <typename Visitor>
  auto apply(Visitor&& vis) -> decltype(auto)
  {
    return ossia::apply(vis, v);
  }

  template <typename Visitor>
  auto apply(Visitor&& vis) const -> decltype(auto)
  {
    return ossia::apply(vis, v);
  }

  bool operator==(const value& rhs) const;
  bool operator!=(const value& rhs) const;
  bool operator>(const value& rhs) const;
  bool operator>=(const value& rhs) const;
  bool operator<(const value& rhs) const;
  bool operator<=(const value& rhs) const;

  template<typename ostream_t>
  friend ostream_t& operator<<(ostream_t& os, const ossia::value& c)
  {
      // TODO OPTIMIZEME
      return os << to_pretty_string(c);
  }
};

inline ossia::value init_value(ossia::val_type type)
{
  switch (type)
  {
    case val_type::IMPULSE:
      return Impulse{};
    case val_type::BOOL:
      return Bool{};
    case val_type::INT:
      return Int{};
    case val_type::FLOAT:
      return Float{};
    case val_type::CHAR:
      return Char{};
    case val_type::STRING:
      return String{};
    case val_type::TUPLE:
      return Tuple{};
    case val_type::VEC2F:
      return Vec2f{};
    case val_type::VEC3F:
      return Vec3f{};
    case val_type::VEC4F:
      return Vec4f{};
    case val_type::DESTINATION:
      return Destination{};
    case val_type::BEHAVIOR:
      return Behavior{{}};
  }

  throw invalid_value_type_error("to_pretty_string: Invalid type");
  return {};
}
}
