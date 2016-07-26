#include "Domain_impl.hpp"

namespace impl
{

JamomaDomain::JamomaDomain(const Value& min,
                           const Value& max,
                           std::vector<Value> values) :
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
  return std::make_shared<JamomaDomain>(*this);
}

JamomaDomain::~JamomaDomain()
{}

# pragma mark -
# pragma mark Accessors

const Value& JamomaDomain::getMin() const
{
  return mMin;
}

Domain & JamomaDomain::setMin(const Value& min)
{
  mMin = min;
  return *this;
}

const Value& JamomaDomain::getMax() const
{
  return mMax;
}

Domain & JamomaDomain::setMax(const Value& max)
{
  mMax = max;
  return *this;
}

const std::vector<Value>& JamomaDomain::getValues() const
{
  return mValues;
}

Domain & JamomaDomain::setValues(const std::vector<Value>& values)
{
  mValues = values;

  return *this;
}

}
