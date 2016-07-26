/*!
 * \file TimeValue.h
 *
 * \defgroup Editor
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include <cmath>
#include <ossia_export.h>
namespace OSSIA
{

class OSSIA_EXPORT TimeValue
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! constructor
   \param int value */
  TimeValue(double d = 0.): m_value{d} { }

  /*! destructor */
  ~TimeValue() = default;

#if 0
# pragma mark -
# pragma mark Operator
#endif

  /*! assignation operator */
  TimeValue & operator= (double d)
  {
    m_value = d;
    return *this;
  }

  TimeValue & operator= (TimeValue t)
  {
      m_value = t.m_value;
      return *this;
  }

  /*! self addition operator */
  TimeValue & operator+= (double d)
  {
    if (isInfinite())
      m_value = 0.;
    else
      m_value += d;

    return *this;
  }

  TimeValue & operator+= (TimeValue t)
  {
    if (isInfinite() || t.isInfinite())
      m_value = 0.;
    else
      m_value += t.m_value;

    return *this;
  }

  /*! self substraction operator */
  TimeValue & operator-= (double d)
  {
    if (isInfinite())
      m_value = 0.;
    else
      m_value -= d;

    return *this;
  }

  TimeValue & operator-= (TimeValue t)
  {
    if (isInfinite() || t.isInfinite())
      m_value = 0.;
    else
      m_value -= t.m_value;

    return *this;
  }


  /*! addition operator */
  TimeValue operator+ (double d) const
  {
    return TimeValue(m_value + d);
  }

  TimeValue operator+ (TimeValue t) const
  {
    if (isInfinite() || t.isInfinite())
    {
      return TimeValue(INFINITY);
    }

    return TimeValue(m_value + t.m_value);
  }

  /*! substraction operator */
  TimeValue operator- (double d) const
  {
    return TimeValue(m_value - d);
  }

  TimeValue operator- (TimeValue t) const
  {
    if (isInfinite() || t.isInfinite())
    {
      return TimeValue(INFINITY);
    }

    return TimeValue(m_value - t.m_value);
  }

  /*! double casting operator */
  operator double() const { return m_value; }

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  /*! is the time value infinite ?
   \return bool infinite */
  bool isInfinite() const
  {
    return std::isinf(m_value);
  }


protected:

  double  m_value;
};

const TimeValue Infinite{INFINITY};
const TimeValue Zero{0.};
const TimeValue One{1.};

}
