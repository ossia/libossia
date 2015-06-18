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

TimeValue::operator double()
{
  return m_value;
}

# pragma mark -
# pragma mark Accessors

bool TimeValue::isInfinite() const
{
  return m_infinite;
}
