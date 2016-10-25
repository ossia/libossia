#pragma once
#include <ossia/editor/dataspace/dataspace_visitors.hpp>
#include <ossia/editor/value/value_traits.hpp>
namespace ossia
{
namespace detail
{

template<typename T, typename U, typename = void>
struct convert_unit_helper
{
  OSSIA_INLINE ossia::value_with_unit operator()(const strong_value<T>& value, const U& unit)
  {
    return {};
  }
};

template<typename T, typename U>
struct convert_unit_helper<T,U, enable_if_same_dataspace<T, U>>
{
  OSSIA_INLINE ossia::value_with_unit operator()(const strong_value<T>& value, const U& unit)
  {
    return strong_value<U>(value);
  }
};

struct convert_unit_visitor
{
  template<typename T, typename U>
  OSSIA_INLINE ossia::value_with_unit operator()(const strong_value<T>& value, const U& unit)
  {
    return convert_unit_helper<T,U>{}(value, unit);
  }

  template<typename... Args1, typename... Args2>
  OSSIA_INLINE ossia::value_with_unit operator()(const eggs::variant<Args1...>& value, const eggs::variant<Args2...>& dataspace)
  {
    return eggs::variants::apply(*this, value, dataspace);
  }

  template<typename... Args2>
  OSSIA_INLINE ossia::value_with_unit operator()(const ossia::value& value, const eggs::variant<Args2...>& dataspace)
  {
    // Just set the unit on the value. This case is handled in value_with_unit and should not be
    // triggered here.
    // Throw if it is incompatible ?
    return {};
  }
};

struct convert_to_value_visitor
{
    template<typename... Args>
    OSSIA_INLINE ossia::value operator()(const eggs::variant<Args...>& value)
    {
      return eggs::variants::apply(*this, value);
    }

    template<typename T>
    OSSIA_INLINE ossia::value operator()(strong_value<T> val)
    {
      return val.dataspace_value;
    }

    OSSIA_INLINE ossia::value operator()(ossia::value val)
    {
      return val;
    }

    OSSIA_INLINE ossia::value operator()()
    {
      return {};
    }
};


struct convert_to_unit_visitor
{
    template<typename... Args>
    OSSIA_INLINE ossia::unit_t operator()(const eggs::variant<Args...>& value)
    {
      return eggs::variants::apply(*this, value);
    }

    template<typename T>
    OSSIA_INLINE ossia::unit_t operator()(const T&)
    {
      return typename T::unit_type{};
    }

    OSSIA_INLINE ossia::unit_t operator()(ossia::value val)
    {
      return {};
    }

    OSSIA_INLINE ossia::unit_t operator()()
    {
      return {};
    }
};

}
}
