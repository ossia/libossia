#include "Editor/TimeValue.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life cycle

TimeValue::TimeValue(double d) :
m_value(d)
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
  return *this;
}

TimeValue & TimeValue::operator= (const TimeValue& t)
{
  m_value = t.m_value;
  return *this;
}

TimeValue& TimeValue::operator+=(double d)
{
  if (isInfinite())
    m_value = 0.;
  else
    m_value += d;
  
  return *this;
}

TimeValue & TimeValue::operator+= (const TimeValue& t)
{
  if (isInfinite() || t.isInfinite())
    m_value = 0.;
  else
    m_value += t.m_value;
  
  return *this;
}

TimeValue& TimeValue::operator-=(double d)
{
  if (isInfinite())
    m_value = 0.;
  else
    m_value -= d;
  
  return *this;
}

TimeValue & TimeValue::operator-= (const TimeValue& t)
{
  if (isInfinite() || t.isInfinite())
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
  if (isInfinite() || t.isInfinite())
  {
    return TimeValue(INFINITY);
  }
  
  return TimeValue(m_value + t.m_value);
}

TimeValue TimeValue::operator- (double d) const
{
  return TimeValue(m_value - d);
}

TimeValue TimeValue::operator- (const TimeValue& t) const
{
  if (isInfinite() || t.isInfinite())
  {
    return TimeValue(INFINITY);
  }
  
  return TimeValue(m_value - t.m_value);
}

TimeValue::operator double() const
{
  return m_value;
}

# pragma mark -
# pragma mark Accessors

bool TimeValue::isInfinite() const
{
  return isinf(m_value);
}
