#pragma once
#include <string>
#include <ossia/detail/string_view.hpp>
#include <ossia/detail/optional.hpp>
#include <boost/any.hpp>
#include <hopscotch_map.h>
#include <ossia_export.h>

namespace ossia
{

struct string_hash
{
  std::size_t operator()(const std::string& s) const
  {
    return std::hash<std::string>{}(s);
  }
  std::size_t operator()(ossia::string_view s) const
  {
    return std::hash<ossia::string_view>{}(s);
  }
};

struct string_equal
{
  using is_transparent = std::true_type;
  bool operator()(const std::string& s, const std::string& s2) const
  {
    return s == s2;
  }
  bool operator()(ossia::string_view s, const std::string& s2) const
  {
    return s == s2;
  }
  bool operator()(const std::string& s, ossia::string_view s2) const
  {
    return s == s2;
  }
};

//! A container to store any kind of data indexed by a string
using any_map =
  tsl::hopscotch_map<
    std::string, boost::any,
    string_hash, string_equal>;

using extended_attributes = any_map;
/**
 * @brief get_attribute Get an attribute of an any_map.
 *
 * Example :
 * \code
 * auto desc = get_attribute<description>(e, "description");
 * \endcode
 *
 * The value will be default-constructed if it is not found (e.g. empty string, 0...)
 */
template<typename T>
auto get_attribute(
    const any_map& e,
    ossia::string_view name)
{
  auto it = e.find(name);
  if(it != e.cend())
  {
    auto val = boost::any_cast<T>(&it.value());
    if(val)
      return *val;
  }

  return T{};
}

/**
 * @brief get_optional_attribute Maybe get an attribute of an any_map.
 *
 * Example :
 * \code
 * auto desc = get_optional_attribute<description>(e, "description");
 * if(desc)
 *   std::cerr << "The description is: " << *desc << "\n";
 * \endcode
 *
 * The value will be an empty \ref optional if it is not found.
 */
template<typename T>
optional<T> get_optional_attribute(
    const any_map& e,
    ossia::string_view name)
{
  auto it = e.find(name);
  if(it != e.cend())
  {
    auto val = boost::any_cast<T>(&it.value());
    if(val)
      return *val;
  }

  return ossia::none;
}

//! Sets an attribute in an any_map
template<typename T>
void set_attribute(any_map& e, ossia::string_view str, const T& val)
{
  // TODO insert_or_assign
  auto it = e.find(str);
  if(it != e.end())
    it.value() = val;
  else
    e.insert(std::make_pair(str.to_string(), val));
}

//! Sets an attribute in an any_map
template<typename T>
void set_attribute(any_map& e, ossia::string_view str, T&& val)
{
  // TODO insert_or_assign
  auto it = e.find(str);
  if(it != e.end())
    it.value() = std::move(val);
  else
    e.insert(std::make_pair(str.to_string(), std::move(val)));
}

//! Removes an attribute in an any_map
inline OSSIA_EXPORT void set_attribute(
    any_map &e,
    ossia::string_view str,
    ossia::none_t)
{
  e.erase(str);
}

//! Sets an attribute if opt has a value, else remove the attribute
template<typename T>
void set_optional_attribute(
    any_map &e,
    ossia::string_view str,
    const optional<T>& opt)
{
  if(opt)
    set_attribute(e, str, *opt);
  else
    set_attribute(e, str, ossia::none);
}
}
