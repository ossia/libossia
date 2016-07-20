#include "Editor/JamomaDomain.h"

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  shared_ptr<Domain> Domain::create(const SafeValue& min,
                                    const SafeValue& max,
                                    std::vector<SafeValue> values)
  {
    return make_shared<JamomaDomain>(min, max, values);
  }

  shared_ptr<Domain> Domain::create(const SafeValue& min,
                                    const SafeValue& max)
  {
    return create(min, max, {});
  }

  shared_ptr<Domain> Domain::create()
  {
    return create(Impulse{}, Impulse{});
  }
}

JamomaDomain::JamomaDomain(const SafeValue& min,
                           const SafeValue& max,
                           std::vector<SafeValue> values) :
  mMin(min),
  mMax(max),
  mValues(values)
{
}

JamomaDomain::JamomaDomain(const JamomaDomain& other) :
mMin(other.mMin),
mMax(other.mMax),
  mValues(other.mValues)
{

}

shared_ptr<Domain> JamomaDomain::clone() const
{
  return make_shared<JamomaDomain>(*this);
}

JamomaDomain::~JamomaDomain()
{}

Domain::~Domain()
{}

# pragma mark -
# pragma mark Accessors

const SafeValue& JamomaDomain::getMin() const
{
  return mMin;
}

Domain & JamomaDomain::setMin(const SafeValue& min)
{
  mMin = min;
  return *this;
}

const SafeValue& JamomaDomain::getMax() const
{
  return mMax;
}

Domain & JamomaDomain::setMax(const SafeValue& max)
{
  mMax = max;
  return *this;
}

const std::vector<SafeValue>& JamomaDomain::getValues() const
{
  return mValues;
}

Domain & JamomaDomain::setValues(const std::vector<SafeValue>& values)
{
  mValues = values;

  return *this;
}

