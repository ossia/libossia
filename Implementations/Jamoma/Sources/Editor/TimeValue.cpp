#include "Editor/TimeValue.h"
using namespace OSSIA;

TimeValue::TimeValue()
{
  // todo : we shouldn't init each time we create an object ...
  TTFoundationInit("/usr/local/jamoma/");
  TTModularInit("/usr/local/jamoma/");
  TTScoreInit("/usr/local/jamoma/");
}

TimeValue::TimeValue(const int c)
{
//	date=c;
}

TimeValue& TimeValue::operator=(const int c)
{
	return *this;
//	date=c;
//	return *this;
}

TimeValue::operator int()
{
	return -1;
//	return date;
}
