#pragma once
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
  constexpr Impulse() = default;
  constexpr Impulse(const Impulse&) = default;
  constexpr Impulse(Impulse&&) = default;
  constexpr Impulse& operator=(const Impulse&) = default;
  constexpr Impulse& operator=(Impulse&&) = default;

  constexpr bool operator==(const value&) const
  {
    return true;
  }
  constexpr bool operator!=(const value&) const
  {
    return false;
  }
  constexpr bool operator>(const value&) const
  {
    return false;
  }
  constexpr bool operator>=(const value&) const
  {
    return true;
  }
  constexpr bool operator<(const value&) const
  {
    return false;
  }
  constexpr bool operator<=(const value&) const
  {
    return true;
  }
};
}
