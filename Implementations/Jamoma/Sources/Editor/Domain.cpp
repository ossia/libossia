#include "Editor/JamomaDomain.h"

# pragma mark -
# pragma mark Life cycle

shared_ptr<Domain> Domain::create(Value * min,
                                  Value * max,
                                  vector<Value*> values)
{
  return make_shared<JamomaDomain>(min, max, values);
}

JamomaDomain::JamomaDomain(Value * min,
                           Value * max,
                           vector<Value*> values) :
mMin(min),
mMax(max),
mValues(values)
{}
  
JamomaDomain::~JamomaDomain()
{}

# pragma mark -
# pragma mark Accessors

Value * JamomaDomain::getMin() const
{
  return mMin;
}

void JamomaDomain::setMin(Value * min)
{
  mMin = min;
}

Value * JamomaDomain::getMax() const
{
  return mMax;
}

void JamomaDomain::setMax(Value * max)
{
  mMax = max;
}

vector<Value*> JamomaDomain::getValues() const
{
  return mValues;
}

void JamomaDomain::setValues(vector<Value*> values)
{
  mValues = values;
}

