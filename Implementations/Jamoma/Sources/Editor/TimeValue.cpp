#include "Editor/TimeValue.h"

using namespace OSSIA;

TimeValue::TimeValue()
{}

TimeValue::TimeValue(const double d)
{
  m_value = d;
}

TimeValue::~TimeValue()
{
    ;
}

TimeValue& TimeValue::operator=(const double d)
{
  m_value = d;
  return *this;
}

TimeValue& TimeValue::operator+=(const double d)
{
  m_value += d;
  return *this;
}

TimeValue& TimeValue::operator-=(const double d)
{
  m_value -= d;
  return *this;
}

TimeValue::operator double()
{
	return m_value;
}
