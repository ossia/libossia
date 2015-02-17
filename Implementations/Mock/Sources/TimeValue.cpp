#include "Editor/TimeValue.h"
using namespace OSSIA;

TimeValue::TimeValue()
{

}

TimeValue::~TimeValue()
{

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
