#pragma once
#include <cmath>
#include <ossia_export.h>
namespace ossia
{

/**
 * @brief The time_value class
 *
 * Represents an amount of time.
 * The unit is context-dependent.
 */
class OSSIA_EXPORT time_value
{

public:
  /*! constructor
   \param int value */
  explicit constexpr time_value(double d) noexcept :
    m_value{d}
  {
  }

  explicit constexpr time_value() noexcept :
    m_value{0.}
  {
  }

  /*! destructor */
  ~time_value() = default;

  /*! assignation operator */
  time_value& operator=(double d) noexcept
  {
    m_value = d;
    return *this;
  }

  time_value& operator=(time_value t) noexcept
  {
    m_value = t.m_value;
    return *this;
  }

  /*! self addition operator */
  time_value& operator+=(double d) noexcept
  {
    if (isInfinite())
      m_value = 0.;
    else
      m_value += d;

    return *this;
  }

  time_value& operator+=(time_value t) noexcept
  {
    if (isInfinite() || t.isInfinite())
      m_value = 0.;
    else
      m_value += t.m_value;

    return *this;
  }

  /*! self substraction operator */
  time_value& operator-=(double d) noexcept
  {
    if (isInfinite())
      m_value = 0.;
    else
      m_value -= d;

    return *this;
  }

  time_value& operator-=(time_value t) noexcept
  {
    if (isInfinite() || t.isInfinite())
      m_value = 0.;
    else
      m_value -= t.m_value;

    return *this;
  }

  /*! addition operator */
  constexpr time_value operator+(double d) const noexcept
  {
    return time_value(m_value + d);
  }

  time_value operator+(time_value t) const noexcept
  {
    if (isInfinite() || t.isInfinite())
    {
      return time_value(INFINITY);
    }

    return time_value(m_value + t.m_value);
  }

  /*! substraction operator */
  constexpr time_value operator-(double d) const noexcept
  {
    return time_value(m_value - d);
  }

  time_value operator-(time_value t) const noexcept
  {
    if (isInfinite() || t.isInfinite())
    {
      return time_value(INFINITY);
    }

    return time_value(m_value - t.m_value);
  }

  /*! double casting operator */
  constexpr operator double() const noexcept
  {
    return m_value;
  }

  /*! is the time value infinite ?
   \return bool infinite */
  bool isInfinite() const noexcept
  {
    return std::isinf(m_value);
  }

  bool operator==(time_value rhs) const noexcept { return m_value == rhs.m_value; }
  bool operator!=(time_value rhs) const noexcept { return m_value != rhs.m_value; }
protected:
  double m_value;
};

OSSIA_EXPORT inline time_value operator "" _tv(long double v)
{
    return time_value(v);
}

const constexpr time_value Infinite{INFINITY};
const constexpr time_value Zero{0.};
const constexpr time_value One{1.};

}
