#pragma once
#include <cmath>
#include <ossia_export.h>
namespace ossia
{

class OSSIA_EXPORT time_value
{

public:
  /*! constructor
   \param int value */
  constexpr time_value(double d = 0.) noexcept :
    m_value{d}
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
  constexpr bool isInfinite() const noexcept
  {
    return std::isinf(m_value);
  }

protected:
  double m_value;
};

const constexpr time_value Infinite{INFINITY};
const constexpr time_value Zero{0.};
const constexpr time_value One{1.};
}
