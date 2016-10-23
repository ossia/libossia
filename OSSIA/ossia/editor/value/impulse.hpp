#pragma once
#include <ossia/detail/config.hpp>
#include <ossia_export.h>

namespace ossia
{
class value;

/**
 * @class Impulse impulse.hpp ossia/editor/value/value.hpp
 *
 * Any value can be converted to an impulse.
 * An impulse generally just means that we want to send a message to the address,
 * and a value is not needed.
 *
 * For instance :
 * \code
 * /audio/player/stop
 * \endcode
 *
 * \see expression_pulse
 */
struct OSSIA_EXPORT Impulse
{
  OSSIA_DECL_RELAXED_CONSTEXPR Impulse() = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Impulse(const Impulse&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Impulse(Impulse&&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Impulse& operator=(const Impulse&) = default;
  OSSIA_DECL_RELAXED_CONSTEXPR Impulse& operator=(Impulse&&) = default;

  OSSIA_DECL_RELAXED_CONSTEXPR bool operator==(ossia::Impulse) const
  {
    return true;
  }

  OSSIA_DECL_RELAXED_CONSTEXPR bool operator!=(ossia::Impulse) const
  {
    return false;
  }

  OSSIA_DECL_RELAXED_CONSTEXPR bool operator>(ossia::Impulse) const
  {
    return false;
  }

  OSSIA_DECL_RELAXED_CONSTEXPR bool operator>=(ossia::Impulse) const
  {
    return true;
  }

  OSSIA_DECL_RELAXED_CONSTEXPR bool operator<(ossia::Impulse) const
  {
    return false;
  }

  OSSIA_DECL_RELAXED_CONSTEXPR bool operator<=(ossia::Impulse) const
  {
    return true;
  }
};

template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator==(const T&, ossia::Impulse)
{
  return true;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator!=(const T&, ossia::Impulse)
{
  return false;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator>(const T&, ossia::Impulse)
{
  return false;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator>=(const T&, ossia::Impulse)
{
  return true;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator<(const T&, ossia::Impulse)
{
  return false;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator<=(const T&, ossia::Impulse)
{
  return true;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator==(ossia::Impulse, const T&)
{
  return true;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator!=(ossia::Impulse, const T&)
{
  return false;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator>(ossia::Impulse, const T&)
{
  return false;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator>=(ossia::Impulse, const T&)
{
  return true;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator<(ossia::Impulse, const T&)
{
  return false;
}
template<typename T>
OSSIA_DECL_RELAXED_CONSTEXPR bool operator<=(ossia::Impulse, const T&)
{
  return true;
}
}
