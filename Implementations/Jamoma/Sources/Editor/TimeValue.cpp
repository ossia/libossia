#include "Editor/TimeValue.h"

using namespace OSSIA;

# pragma mark -
# pragma mark Life cycle

TimeValue::TimeValue(const bool infinite) :
m_infinite(infinite)
{}

TimeValue::TimeValue(const double d, const bool infinite) :
m_infinite(infinite)
{
  m_value = d;
  
  if (m_infinite)
    m_value = 0.;
}

TimeValue::~TimeValue()
{
    ;
}

# pragma mark -
# pragma mark Operator

TimeValue& TimeValue::operator=(const double d)
{
  m_value = d;
  
  if (m_infinite)
    m_value = 0.;
  
  return *this;
}

TimeValue& TimeValue::operator+=(const double d)
{
  m_value += d;
  
  if (m_infinite)
    m_value = 0.;
  
  return *this;
}

TimeValue& TimeValue::operator-=(const double d)
{
  m_value -= d;
  
  if (m_infinite)
    m_value = 0.;
  
  return *this;
}

bool TimeValue::operator== (const TimeValue& t)
{
  return m_infinite ? t.m_infinite : (t.m_infinite ? false : m_value == t.m_value);
}

bool TimeValue::operator!= (const TimeValue& t)
{
  return !(*this == t);
}

bool TimeValue::operator> (const TimeValue& t)
{
  return m_infinite ? !t.m_infinite : (t.m_infinite ? false : m_value > t.m_value);
}

bool TimeValue::operator>= (const TimeValue& t)
{
  return m_infinite ? t.m_infinite : (t.m_infinite ? false : m_value >= t.m_value);
}

bool TimeValue::operator< (const TimeValue& t)
{
  return t >= *this;
}

bool TimeValue::operator<= (const TimeValue& t)
{
  return t > *this;
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
