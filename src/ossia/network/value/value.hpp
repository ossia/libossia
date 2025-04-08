#pragma once
#include <ossia/detail/concepts.hpp>
#include <ossia/detail/destination_index.hpp>
#include <ossia/detail/string_view.hpp>
#include <ossia/network/common/parameter_properties.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/value/value_base.hpp>

#include <boost/container/flat_map.hpp>

#include <limits>
#include <string>
#include <vector>

namespace ossia
{
namespace detail
{
template <typename T>
struct dummy
{
  using type = T;
};
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
 *     "list: [ int: 2, float: 3 ]"
 * etc...
 *
 */
OSSIA_EXPORT std::string value_to_pretty_string(const ossia::value& val);
OSSIA_EXPORT ossia::value parse_pretty_value(std::string_view str);

using value_map_element = std::pair<std::string, ossia::value>;
struct value_map_type : std::vector<value_map_element>
{
  using vector::vector;

  OSSIA_EXPORT
  ossia::value& operator[](std::string_view str) noexcept;
};

struct value_variant_type
{
public:
  struct dummy_t
  {
  };
  union Impl
  {
    float m_value0;

    int32_t m_value1;

    ossia::vec2f m_value2;

    ossia::vec3f m_value3;

    ossia::vec4f m_value4;

    ossia::impulse m_value5;

    bool m_value6;

    std::string m_value7;

    std::vector<ossia::value> m_value8;

    value_map_type m_value9;

    dummy_t m_dummy;
    Impl()
        : m_dummy{}
    {
    }
    ~Impl() { }
  };

  enum Type : int8_t
  {
    Type0,
    Type1,
    Type2,
    Type3,
    Type4,
    Type5,
    Type6,
    Type7,
    Type8,
    Type9,
    Npos = std::numeric_limits<int8_t>::max()
  };

  void destruct_impl();
  Impl m_impl;
  Type m_type;

public:
  static const constexpr auto npos = Npos;
  int which() const;

  operator bool() const;
  template <typename T>
  const T* target() const;
  template <typename T>
  T* target();
  template <typename T>
  const T& get() const;
  template <typename T>
  T& get();

  template <typename T>
  static Type matching_type();

  value_variant_type();
  ~value_variant_type();
  value_variant_type(float v);
  value_variant_type(int32_t v);
  value_variant_type(ossia::vec2f v);
  value_variant_type(ossia::vec3f v);
  value_variant_type(ossia::vec4f v);
  value_variant_type(ossia::impulse v);
  value_variant_type(bool v);
  value_variant_type(const std::string& v);
  value_variant_type(std::string&& v);

  value_variant_type(const std::vector<ossia::value>& v);
  value_variant_type(std::vector<ossia::value>&& v) noexcept;
  value_variant_type(const value_map_type& v);
  value_variant_type(value_map_type&& v);
  value_variant_type(const value_variant_type& other);
  value_variant_type(value_variant_type&& other) noexcept;
  value_variant_type& operator=(const value_variant_type& other);
  value_variant_type& operator=(value_variant_type&& other) noexcept;
};
using value_variant = value_variant_type;

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
  value(T*) = delete;
  value(const char* txt)
      : v{std::string(txt)}
  {
  }

  OSSIA_INLINE value(impulse val) noexcept
      : v{val}
  {
  }

  OSSIA_INLINE value(bool val) noexcept
      : v{val}
  {
  }
  OSSIA_INLINE value(int val) noexcept
      : v{(int32_t)val}
  {
  }
  OSSIA_INLINE value(long val) noexcept
      : v{(int32_t)val}
  {
  }
  OSSIA_INLINE value(float val) noexcept
      : v{val}
  {
  }
  OSSIA_INLINE value(double val) noexcept
      : v{(float)val}
  {
  }
  OSSIA_INLINE value(const std::string& val) noexcept
      : v{val}
  {
  }

  OSSIA_INLINE value(const std::vector<ossia::value>& val) noexcept
      : v(val)
  {
  }

  OSSIA_INLINE value(const value_map_type& val) noexcept
      : v(val)
  {
  }
  OSSIA_INLINE value(std::array<float, 2> val) noexcept
      : v(val)
  {
  }
  OSSIA_INLINE value(std::array<float, 3> val) noexcept
      : v(val)
  {
  }
  OSSIA_INLINE value(std::array<float, 4> val) noexcept
      : v(val)
  {
  }

  OSSIA_INLINE value(std::string&& val) noexcept
      : v(std::move(val))
  {
  }
  OSSIA_INLINE value(std::vector<ossia::value>&& val) noexcept
      : v(std::move(val))
  {
  }

  OSSIA_INLINE value(value_map_type&& val) noexcept
      : v(std::move(val))
  {
  }

  template <typename T, typename... Args>
  OSSIA_INLINE value(detail::dummy<T> t, Args&&... args) noexcept
      : v(T(std::forward<Args>(args)...))
  {
  }

  // To initialize a value directly with correct arguments
  template <typename T, typename... Args>
  OSSIA_INLINE static ossia::value make(Args&&... args) noexcept
  {
    return ossia::value{detail::dummy<T>{}, std::forward<Args>(args)...};
  }

  // Assignment
  OSSIA_INLINE value& operator=(ossia::impulse val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(const char* c) noexcept
  {
    v = std::string(c);
    return *this;
  }
  OSSIA_INLINE value& operator=(bool val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(int32_t val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(float val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(const std::string& val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(const std::vector<ossia::value>& val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(const value_map_type& val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(std::array<float, 2> val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(std::array<float, 3> val) noexcept
  {
    v = val;
    return *this;
  }
  OSSIA_INLINE value& operator=(std::array<float, 4> val) noexcept
  {
    v = val;
    return *this;
  }

  OSSIA_INLINE value& operator=(std::string&& val) noexcept
  {
    v = std::move(val);
    return *this;
  }
  OSSIA_INLINE value& operator=(std::vector<ossia::value>&& val) noexcept
  {
    v = std::move(val);
    return *this;
  }
  OSSIA_INLINE value& operator=(value_map_type&& val) noexcept
  {
    v = std::move(val);
    return *this;
  }

  OSSIA_INLINE value() noexcept { }
  ~value() noexcept;
  value(const value& other) noexcept
      : v(other.v)
  {
  }
  value(value&& other) noexcept
      : v(std::move(other.v))
  {
  }
  value& operator=(const value& other) noexcept
  {
    v = other.v;
    return *this;
  }
  value& operator=(value&& other) noexcept
  {
    v = std::move(other.v);
    return *this;
  }

  operator value_type&() { return v; }
  operator const value_type&() const { return v; }

  // Operations
  template <typename T>
  OSSIA_INLINE const T& get() const
  {
    return v.get<T>();
  }

  template <typename T>
  OSSIA_INLINE T& get()
  {
    return v.get<typename std::remove_const<T>::type>();
  }

  template <typename T>
  OSSIA_INLINE const T* target() const& noexcept
  {
    using type = typename std::remove_const<T>::type;
    static_assert(!std::is_same<type, ossia::value>::value, "");
    return v.target<type>();
  }

  template <typename T>
  OSSIA_INLINE T* target() & noexcept
  {
    using type = typename std::remove_const<T>::type;
    static_assert(!std::is_same<type, ossia::value>::value, "");
    return v.target<type>();
  }

  template <typename T>
  OSSIA_INLINE T* target() && noexcept = delete;

  OSSIA_INLINE ossia::val_type get_type() const noexcept
  {
    auto t = v.which();
    if(t == v.npos)
    {
      return ossia::val_type::NONE;
    }

    return static_cast<ossia::val_type>(t);
  }

  bool valid() const noexcept { return bool(v); }

  void reset() noexcept { v = value_type{}; }

  template <typename Visitor>
  auto apply(Visitor&& vis) -> decltype(auto);

  template <typename Visitor>
  auto apply(Visitor&& vis) const -> decltype(auto);

  friend OSSIA_EXPORT bool operator==(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator!=(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator>(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator>=(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator<(const value& lhs, const value& rhs);
  friend OSSIA_EXPORT bool operator<=(const value& lhs, const value& rhs);
  /*
    friend std::ostream& operator<<(std::ostream& os, const ossia::value& c)
    {
      // TODO OPTIMIZEME
      return os << value_to_pretty_string(c);
    }

    friend std::istream& operator>>(std::istream& is, const ossia::value& c)
    {
      // TODO
      return is;
    }
    */
};

inline ossia::value init_value(ossia::val_type type)
{
  switch(type)
  {
    case val_type::IMPULSE:
      return ossia::impulse{};
    case val_type::BOOL:
      return bool{};
    case val_type::INT:
      return int32_t{};
    case val_type::FLOAT:
      return float{};
    case val_type::STRING:
      return value{std::string{}}; // value needed for explicit ctor
    case val_type::LIST:
      return value{std::vector<ossia::value>{}};
    case val_type::MAP:
      return value{value_map_type{}};
    case val_type::VEC2F:
      return vec2f{};
    case val_type::VEC3F:
      return vec3f{};
    case val_type::VEC4F:
      return vec4f{};
    case val_type::NONE:
    default:
      break;
  }

  ossia_do_throw(invalid_value_type_error, "init_value: Invalid type");
  return {};
}

/**
 * @brief get_value_at_index
 *
 * for ossia::list{{1, 2}, 3} and index {}, return {{1, 2}, 3}.
 * for ossia::list{{1, 2}, 3} and index {0}, return {1, 2}.
 * for ossia::list{{1, 2}, 3} and index {0, 0}, return 1.
 * for ossia::list{{1, 2}, 3} and index {0, 1}, return 2.
 * for ossia::list{{1, 2}, 3} and index {0, 2}, return {}.
 * for ossia::list{{1, 2}, 3} and index {1}, return 3.
 * for ossia::list{{1, 2}, 3} and index {1, 0}, return {}.
 * for ossia::list{{1, 2}, 3} and index {2}, return {}.
 *
 * @see @ref destination_index
 * @return The value if it is found, else nothing.
 */
OSSIA_EXPORT
ossia::value
get_value_at_index(const ossia::value& val, const ossia::destination_index& idx);

#include <ossia/network/value/value_variant_impl.hpp>

#if defined(OSSIA_HAS_CONCEPTS)
template <typename T>
concept ossia_visitor = requires(T t) {
                          t();
                          t(std::declval<float&>());
                          t(std::declval<int32_t&>());
                          t(std::declval<ossia::vec2f&>());
                          t(std::declval<ossia::vec3f&>());
                          t(std::declval<ossia::vec4f&>());
                          t(std::declval<ossia::impulse&>());
                          t(std::declval<bool&>());
                          t(std::declval<std::string&>());
                          t(std::declval<std::vector<ossia::value>&>());
                          t(std::declval<value_map_type&>());
                        };
#endif

template <typename Visitor>
inline auto value::apply(Visitor&& vis) -> decltype(auto)
{
#if defined(OSSIA_HAS_CONCEPTS)
  static_assert(ossia_visitor<Visitor>, "Not a valid ossia::value visitor");
#endif
  return ossia::apply(std::forward<Visitor>(vis), this->v);
}

template <typename Visitor>
inline auto value::apply(Visitor&& vis) const -> decltype(auto)
{
#if defined(OSSIA_HAS_CONCEPTS)
  static_assert(ossia_visitor<Visitor>, "Not a valid ossia::value visitor");
#endif
  return ossia::apply(std::forward<Visitor>(vis), this->v);
}
}

namespace std
{
OSSIA_EXPORT std::ostream&
operator<<(std::ostream&, const std::vector<std::string>& list);
OSSIA_EXPORT std::istream& operator>>(std::istream&, std::vector<std::string>& list);
}
/*
extern template class std::vector<ossia::value>;
#if defined(OSSIA_USE_BOOST_OPTIONAL)
extern template class boost::optional<ossia::value>;
#elif defined(OSSIA_USE_STD_OPTIONAL)
extern template class std::optional<ossia::value>;
#elif defined(OSSIA_USE_STD_EXPERIMENTAL_OPTIONAL)
extern template class std::experimental::optional<ossia::value>;
#endif
*/
