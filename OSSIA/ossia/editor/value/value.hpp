#pragma once
#include <ossia/editor/value/value_base.hpp>
#include <ossia/editor/exceptions.hpp>

#include <eggs/variant.hpp>

#include <ossia_export.h>

namespace ossia
{
namespace detail
{
template<typename T>
struct dummy { using type = T; };
}
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
  constexpr value(T*) = delete;
  constexpr value(ossia::Impulse val) : v{val} { }
  constexpr value(ossia::Bool val) : v{val} { }
  constexpr value(ossia::Int val) : v{val} { }
  constexpr value(ossia::Float val) : v{val} { }
  constexpr value(ossia::Char val) : v{val} { }
  constexpr value(const ossia::String& val) : v{val} { }
  constexpr value(const ossia::Tuple& val) : v{val} { }
  constexpr value(const ossia::Vec2f& val) : v{val} { }
  constexpr value(const ossia::Vec3f& val) : v{val} { }
  constexpr value(const ossia::Vec4f& val) : v{val} { }
  constexpr value(const ossia::Destination& val) : v{val} { }
  constexpr value(ossia::net::address_base& val) : v{eggs::variants::in_place<ossia::Destination>, val} { }
  constexpr value(const ossia::Behavior& val) : v{val} { }

  template<typename T, typename... Args>
  constexpr value(detail::dummy<T> t, Args&&... args):
    v{eggs::variants::in_place<typename detail::dummy<T>::type>, std::forward<Args>(args)...}
  {

  }

  template<typename T, typename... Args>
  static ossia::value make(Args&&... args)
  { return ossia::value{detail::dummy<T>{}, std::forward<Args>(args)...}; }

  // Movable overloads
  constexpr value(ossia::String&& val) : v{std::move(val)} { }
  constexpr value(ossia::Tuple&& val) : v{std::move(val)} { }
  constexpr value(ossia::Destination&& val) : v{std::move(val)} { }
  constexpr value(ossia::Behavior&& val) : v{std::move(val)} { }


  // Assignment
  value& operator=(ossia::Impulse val)
  {
    v = val;
    return *this;
  }
  value& operator=(ossia::Bool val)
  {
    v = val;
    return *this;
  }
  value& operator=(ossia::Int val)
  {
    v = val;
    return *this;
  }
  value& operator=(ossia::Float val)
  {
    v = val;
    return *this;
  }
  value& operator=(ossia::Char val)
  {
    v = val;
    return *this;
  }
  value& operator=(const ossia::String& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const ossia::Tuple& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const ossia::Vec2f& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const ossia::Vec3f& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const ossia::Vec4f& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const ossia::Destination& val)
  {
    v = val;
    return *this;
  }
  value& operator=(const ossia::Behavior& val)
  {
    v = val;
    return *this;
  }

  // Movable overloads
  value& operator=(ossia::String&& val)
  {
    v = std::move(val);
    return *this;
  }
  value& operator=(ossia::Tuple&& val)
  {
    v = std::move(val);
    return *this;
  }
  value& operator=(ossia::Destination&& val)
  {
    v = std::move(val);
    return *this;
  }
  value& operator=(ossia::Behavior&& val)
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
    case val_type::BEHAVIOR:
      return Behavior{{}};
    case val_type::DESTINATION:
      throw invalid_value_type_error("init_value: do not create Destination like this");
  }

  throw invalid_value_type_error("init_value: Invalid type");
  return {};
}

/**
 * @brief get_value_at_index
 *
 * for ossia::tuple{{1, 2}, 3} and index {}, return {{1, 2}, 3}.
 * for ossia::tuple{{1, 2}, 3} and index {0}, return {1, 2}.
 * for ossia::tuple{{1, 2}, 3} and index {0, 0}, return 1.
 * for ossia::tuple{{1, 2}, 3} and index {0, 1}, return 2.
 * for ossia::tuple{{1, 2}, 3} and index {0, 2}, return {}.
 * for ossia::tuple{{1, 2}, 3} and index {1}, return 3.
 * for ossia::tuple{{1, 2}, 3} and index {1, 0}, return {}.
 * for ossia::tuple{{1, 2}, 3} and index {2}, return {}.
 *
 * @see @ref destination_index
 * @return The value if it is found, else nothing.
 */
ossia::value get_value_at_index(
    const ossia::value& val,
    const ossia::destination_index& idx);
}
