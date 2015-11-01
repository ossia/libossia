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

JamomaDomain::JamomaDomain(const JamomaDomain * other) :
mMin(other->mMin->clone()),
mMax(other->mMax->clone())
{
  for (const auto& e : other->mValues)
    mValues.push_back(e->clone());
}

shared_ptr<Domain> JamomaDomain::clone() const
{
  return make_shared<JamomaDomain>(this);
}

JamomaDomain::~JamomaDomain()
{}

# pragma mark -
# pragma mark Accessors

const Value * JamomaDomain::getMin() const
{
  return mMin;
}

Domain & JamomaDomain::setMin(const Value * min)
{
  mMin = min->clone();
  return *this;
}

const Value * JamomaDomain::getMax() const
{
  return mMax;
}

Domain & JamomaDomain::setMax(const Value * max)
{
  mMax = max->clone();
  return *this;
}

vector<const Value*> JamomaDomain::getValues() const
{
  return mValues;
}

Domain & JamomaDomain::setValues(vector<const Value*> values)
{
  mValues.clear();

  for (const auto& e : values)
    mValues.push_back(e->clone());

  return *this;
}

