#include "Editor/TimeValue.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life cycle

TimeValue::TimeValue(bool infinite) :
m_value(0.),
m_infinite(infinite)
{}

TimeValue::TimeValue(double d) :
m_value(d),
m_infinite(false)
{}

TimeValue::~TimeValue()
{
    ;
}

# pragma mark -
# pragma mark Operator

TimeValue& TimeValue::operator=(double d)
{
  m_value = d;
  m_infinite = false;
  
  return *this;
}

TimeValue & TimeValue::operator= (const TimeValue& t)
{
  m_value = t.m_value;
  m_infinite = t.m_infinite;
  
  return *this;
}

TimeValue& TimeValue::operator+=(double d)
{
  if (m_infinite)
    m_value = 0.;
  else
    m_value += d;
  
  return *this;
}

TimeValue & TimeValue::operator+= (const TimeValue& t)
{
  if (m_infinite || t.m_infinite)
    m_value = 0.;
  else
    m_value += t.m_value;
  
  return *this;
}

TimeValue& TimeValue::operator-=(double d)
{
  if (m_infinite)
    m_value = 0.;
  else
    m_value -= d;
  
  return *this;
}

TimeValue & TimeValue::operator-= (const TimeValue& t)
{
  if (m_infinite || t.m_infinite)
    m_value = 0.;
  else
    m_value -= t.m_value;
  
  return *this;
}

TimeValue TimeValue::operator+ (double d) const
{
  return TimeValue(m_value + d);
}

TimeValue TimeValue::operator+ (const TimeValue& t) const
{
  if (m_infinite || t.m_infinite)
  {
    return TimeValue(true);
  }
  
  return TimeValue(m_value + t.m_value);
}

TimeValue TimeValue::operator- (double d) const
{
  return TimeValue(m_value - d);
}

TimeValue TimeValue::operator- (const TimeValue& t) const
{
  if (m_infinite || t.m_infinite)
  {
    return TimeValue(true);
  }
  
  return TimeValue(m_value - t.m_value);
}

bool TimeValue::operator== (double d) const
{
  return m_infinite ? false : m_value == d;
}

bool TimeValue::operator== (const TimeValue& t) const
{
  return m_infinite ? t.m_infinite : (t.m_infinite ? false : m_value == t.m_value);
}

bool TimeValue::operator!= (double d) const
{
  return m_infinite ? true : m_value != d;
}

bool TimeValue::operator!= (const TimeValue& t) const
{
  return !(*this == t);
}

bool TimeValue::operator> (double d) const
{
  return m_infinite ? true : m_value > d;
}

bool TimeValue::operator> (const TimeValue& t) const
{
  return m_infinite ? !t.m_infinite : (t.m_infinite ? false : m_value > t.m_value);
}

bool TimeValue::operator>= (double d) const
{
  return m_infinite ? true : m_value >= d;
}

bool TimeValue::operator>= (const TimeValue& t) const
{
  return m_infinite ? true : (t.m_infinite ? false : m_value >= t.m_value);
}

bool TimeValue::operator< (double d) const
{
  return m_infinite ? false : m_value < d;
}

bool TimeValue::operator< (const TimeValue& t) const
{
  return m_infinite ? false : (t.m_infinite ? true : m_value < t.m_value);
}

bool TimeValue::operator<= (double d) const
{
  return m_infinite ? false : m_value <= d;
}

bool TimeValue::operator<= (const TimeValue& t) const
{
  return m_infinite ? t.m_infinite : (t.m_infinite ? true : m_value <= t.m_value);
}

TimeValue::operator double() const
{
  return m_value;
}

# pragma mark -
# pragma mark Accessors

bool TimeValue::isInfinite() const
{
  return m_infinite;
}
