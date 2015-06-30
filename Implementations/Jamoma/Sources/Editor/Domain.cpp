#include "Editor/JamomaDomain.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Domain> Domain::create(const Value * min,
                                    const Value * max,
                                    vector<const Value*> values)
  {
    return make_shared<JamomaDomain>(min, max, values);
  }
}

JamomaDomain::JamomaDomain(const Value * min,
                           const Value * max,
                           vector<const Value*> values) :
mMin(min->clone()),
mMax(max->clone())
{
  for (const auto& e : values)
    mValues.push_back(e->clone());
}
  
JamomaDomain::~JamomaDomain()
{}

# pragma mark -
# pragma mark Accessors

const Value * JamomaDomain::getMin() const
{
  return mMin;
}

void JamomaDomain::setMin(const Value * min)
{
  mMin = min->clone();
}

const Value * JamomaDomain::getMax() const
{
  return mMax;
}

void JamomaDomain::setMax(const Value * max)
{
  mMax = max->clone();
}

vector<const Value*> JamomaDomain::getValues() const
{
  return mValues;
}

void JamomaDomain::setValues(vector<const Value*> values)
{
  mValues.clear();
  
  for (const auto& e : values)
    mValues.push_back(e->clone());
}

