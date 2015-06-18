#include "Editor/TimeValue.h"

using namespace OSSIA;

TimeValue::TimeValue()
{}

TimeValue::TimeValue(const double d)
{
//	date=c;
}

TimeValue::~TimeValue()
{
    ;
}

TimeValue& TimeValue::operator=(const double d)
{
	return *this;
//	date=c;
//	return *this;
}

TimeValue& TimeValue::operator+(const double d)
{
  return *this;
  //	date=c;
  //	return *this;
}

TimeValue& TimeValue::operator-(const double d)
{
  return *this;
  //	date=c;
  //	return *this;
}

TimeValue::operator double()
{
	return -1;
//	return date;
}
