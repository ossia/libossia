#pragma once
#include <ossia/editor/value/value_base.hpp>
#include <ossia/editor/exceptions.hpp>
#include <eggs/variant.hpp>

#include <ossia_export.h>

namespace fmt
{
template<typename T, typename Allocator>
class BasicMemoryWriter;
using MemoryWriter = BasicMemoryWriter<char, std::allocator<char>>;
}

namespace ossia
{
namespace detail
{
template<typename T>
struct dummy { using type = T; };
}

/**
 * \brief ossia::apply : helper function to apply a visitor to a variant
 * without throwing in the empty variant case.
 *
 * By default, a eggs::variant throws bad_variant.
 * In this case, the operator()() without arguments will be called.
 * This allows a simpler handling of the default case.
 */
template <typename Visitor, typename Variant>
auto apply(Visitor&& v, Variant&& var) -> decltype(std::forward<Visitor>(v)())
{
  // Thanks K-Ballo (eggs-cpp/variant#21)
  if (var)
    return eggs::variants::apply(
        std::forward<Visitor>(v),
        std::forward<Variant>(var));
  else
    return std::forward<Visitor>(v)();
}


OSSIA_EXPORT std::string to_pretty_string(const ossia::destination_index& index);

class value;

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
OSSIA_EXPORT std::string value_to_pretty_string(const ossia::value& val);

using value_variant_type = eggs::variant<
    Impulse,
    bool, int, float, char,
    std::string,
    std::vector<ossia::value>,
    std::array<float, 2ul>, std::array<float, 3ul>, std::array<float, 4ul>,
    Destination>;

#if defined(_MSC_VER)
using value_variant = value_variant_type;
#else
struct OSSIA_EXPORT value_variant : public value_variant_type
{
    using value_variant_type::value_variant_type;
};
#endif

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
 *     auto maybe_int = v.target<Int>();
 *     if(maybe_int)
 *       std::cout << maybe_int->value;
 *     else // we know for some reason that it is a float
 *       std::cout << v.get<Float>().value;
 *   }
 * }
 * \endcode
 *
 * A generic operation can be applied safely to a value with a visitor.
 * See for instance \ref value_to_pretty_string.
 */
class OSSIA_EXPORT value
{
public:
  using value_type = value_variant;

  value_type v;

  // Construction
  template <typename T>
  OSSIA_DECL_RELAXED_CONSTEXPR value(T*) = delete;
  template <int N>
  OSSIA_DECL_RELAXED_CONSTEXPR value(const char (&txt)[N]):
      v{eggs::variants::in_place<std::string>, txt, N}
  {

  }

  OSSIA_DECL_RELAXED_CONSTEXPR value(Impulse val) noexcept : v{val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(const ossia::Destination& val) noexcept : v{val} { }

  OSSIA_DECL_RELAXED_CONSTEXPR value(bool val) noexcept : v{eggs::variants::in_place<bool>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(int val) noexcept : v{eggs::variants::in_place<int32_t>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(char val) noexcept : v{eggs::variants::in_place<char>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(float val) noexcept : v{eggs::variants::in_place<float>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(double val) noexcept : v{eggs::variants::in_place<float>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(const std::string& val) noexcept : v{eggs::variants::in_place<std::string>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(const std::vector<ossia::value>& val) noexcept : v{eggs::variants::in_place<std::vector<ossia::value>>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(std::array<float, 2> val) noexcept : v{eggs::variants::in_place<ossia::Vec2f>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(std::array<float, 3> val) noexcept : v{eggs::variants::in_place<ossia::Vec3f>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(std::array<float, 4> val) noexcept : v{eggs::variants::in_place<ossia::Vec4f>, val} { }
  OSSIA_DECL_RELAXED_CONSTEXPR value(ossia::net::address_base& val) noexcept : v{eggs::variants::in_place<ossia::Destination>, val} { }

  // Movable overloads
  OSSIA_DECL_RELAXED_CONSTEXPR value(ossia::Destination&& val) noexcept : v{std::move(val)} { }

  OSSIA_DECL_RELAXED_CONSTEXPR explicit value(std::string&& val) noexcept : v{eggs::variants::in_place<std::string>, std::move(val)} { }
  OSSIA_DECL_RELAXED_CONSTEXPR explicit value(std::vector<ossia::value>&& val) noexcept : v{eggs::variants::in_place<std::vector<ossia::value>>, std::move(val)} { }


  template<typename T, typename... Args>
  OSSIA_DECL_RELAXED_CONSTEXPR value(detail::dummy<T> t, Args&&... args) noexcept :
    v{eggs::variants::in_place<typename detail::dummy<T>::type>, std::forward<Args>(args)...}
  {

  }

  // To initialize a value directly with correct arguments
  template<typename T, typename... Args>
  static ossia::value make(Args&&... args) noexcept
  { return ossia::value{detail::dummy<T>{}, std::forward<Args>(args)...}; }

  // Assignment
  value& operator=(ossia::Impulse val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(const ossia::Destination& val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(bool val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(int val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(float val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(char val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(const std::string& val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(const std::vector<ossia::value>& val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(std::array<float, 2> val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(std::array<float, 3> val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(std::array<float, 4> val) noexcept
  {
    v = val;
    return *this;
  }
  value& operator=(ossia::net::address_base& val) noexcept
  {
    v = Destination{val};
    return *this;
  }

  value& operator=(std::string&& val) noexcept
  {
    v = std::move(val);
    return *this;
  }
  value& operator=(std::vector<ossia::value>&& val) noexcept
  {
    v = std::move(val);
    return *this;
  }

  value() noexcept { }
  ~value() noexcept;
  value(const value& other) noexcept : v{other.v} { }
  value(value&& other) noexcept : v{std::move(other.v)} {}
  value& operator=(const value& other) noexcept { v = other.v; return *this; }
  value& operator=(value&& other) noexcept { v = std::move(other.v); return *this; }

  operator value_type&() { return v; }
  operator const value_type&() const { return v; }

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
  const T* target() const noexcept
  {
    return v.target<T>();
  }

  template <typename T>
  T* target()  noexcept
  {
    return v.target<T>();
  }

  ossia::val_type getType() const
  {
    auto t = v.which();
    if (t == v.npos)
    {
      throw ossia::invalid_value_type_error{"value::getType: "
                                            "called with no value"};
      return {};
    }

    return static_cast<ossia::val_type>(t);
  }

  bool valid() const noexcept
  {
    return bool(v);
  }

  void reset() noexcept
  {
    v = value_type{};
  }

  template <typename Visitor>
  auto apply(Visitor&& vis) -> decltype(auto)
  {
    return ossia::apply(std::forward<Visitor>(vis), v);
  }

  template <typename Visitor>
  auto apply(Visitor&& vis) const -> decltype(auto)
  {
    return ossia::apply(std::forward<Visitor>(vis), v);
  }

  friend OSSIA_EXPORT bool operator==(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator!=(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator>(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator>=(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator<(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator<=(const value& lhs, const value& rhs);

  template<typename ostream_t>
  friend ostream_t& operator<<(ostream_t& os, const ossia::value& c)
  {
      // TODO OPTIMIZEME
      return os << value_to_pretty_string(c);
  }
};

inline ossia::value init_value(ossia::val_type type)
{
  switch (type)
  {
    case val_type::IMPULSE:
      return Impulse{};
    case val_type::BOOL:
      return bool{};
    case val_type::INT:
      return int32_t{};
    case val_type::FLOAT:
      return float{};
    case val_type::CHAR:
      return char{};
    case val_type::STRING:
      return std::string{};
    case val_type::TUPLE:
      return std::vector<ossia::value>{};
    case val_type::VEC2F:
      return Vec2f{};
    case val_type::VEC3F:
      return Vec3f{};
    case val_type::VEC4F:
      return Vec4f{};
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

fmt::MemoryWriter& operator<<(fmt::MemoryWriter&, const std::vector<ossia::value>& tuple);
fmt::MemoryWriter& operator<<(fmt::MemoryWriter&, const std::array<float, 2ul>& tuple);
fmt::MemoryWriter& operator<<(fmt::MemoryWriter&, const std::array<float, 3ul>& tuple);
fmt::MemoryWriter& operator<<(fmt::MemoryWriter&, const std::array<float, 4ul>& tuple);
std::ostream& operator<<(std::ostream&, const std::vector<std::string>& tuple);
