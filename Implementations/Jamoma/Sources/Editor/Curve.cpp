#include "Editor/JamomaCurve.h"
#include "Editor/CurveSegment/JamomaCurveSegmentLinear.h"

#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  // explicit instantiation for bool
  template class Curve<bool>;
  
  template <>
  shared_ptr<Curve<bool>> Curve<bool>::create()
  {
    return make_shared<JamomaCurve<bool>>();
  }
  
  // explicit instantiation for int
  template class Curve<int>;
  
  template <>
  shared_ptr<Curve<int>> Curve<int>::create()
  {
    return make_shared<JamomaCurve<int>>();
  }
  
  // explicit instantiation for float
  template class Curve<float>;
  
  template <>
  shared_ptr<Curve<float>> Curve<float>::create()
  {
    return make_shared<JamomaCurve<float>>();
  }
}

template <typename T>
JamomaCurve<T>::JamomaCurve()
{
  mInitialDestinationIndex.push_back(0);
}

template <typename T>
JamomaCurve<T>::JamomaCurve(const JamomaCurve * other)
{}

template <typename T>
shared_ptr<Curve<T>> JamomaCurve<T>::clone() const
{
  return make_shared<JamomaCurve>(this);
}

template <typename T>
JamomaCurve<T>::~JamomaCurve()
{}

# pragma mark -
# pragma mark Edition

template <typename T>
bool JamomaCurve<T>::addPoint(const TimeValue& abscissa, T value, shared_ptr<CurveSegment<T>> segment)
{
  pair<T,shared_ptr<CurveSegment<T>>> p(value, segment);
  
  //! \todo check if there is already a point
  
  mPointsMap.emplace(abscissa, p);
  
  return true;
}

template <typename T>
bool JamomaCurve<T>::removePoint(const TimeValue& abscissa)
{
  return mPointsMap.erase(abscissa) > 0;
}

# pragma mark -
# pragma mark Execution

template <typename T>
T JamomaCurve<T>::valueAt(const TimeValue& abscissa) const
{
  TimeValue lastAbscissa(0.);
  T lastValue = getInitialValue();
  
  for (auto it = mPointsMap.begin(); it != mPointsMap.end(); it++)
  {
    if (abscissa > lastAbscissa &&
        abscissa <= it->first)
    {      
      lastValue = it->second.second->valueAt((abscissa - lastAbscissa) / (it->first - lastAbscissa), lastValue , it->second.first);
      break;
    }
    else if (abscissa > it->first)
    {
      lastAbscissa = it->first;
      lastValue = it->second.first;
    }
    else
      break;
  }
  
  return lastValue;
}

# pragma mark -
# pragma mark Accessors

template <typename T>
T JamomaCurve<T>::getInitialValue() const
{
  if (mInitialDestination == nullptr)
  {
    return mInitialValue;
  }
  else
  {
    auto address = mInitialDestination->value->getAddress();
    
    if (!address)
      throw runtime_error("getting an address value using from a destination without address");
    
    char level = 0;
    return convertToTemplateTypeValue(address->pullValue(), &level);
  }
}

template <typename T>
void JamomaCurve<T>::setInitialValue(const T value)
{
  mInitialValue = value;
}

template <typename T>
const Destination* JamomaCurve<T>::getInitialDestination() const
{
  return mInitialDestination;
}

template <typename T>
void JamomaCurve<T>::setInitialDestination(const Destination* destination)
{
  mInitialDestination = static_cast<Destination*>(destination->clone());
}

template <typename T>
vector<char> JamomaCurve<T>::getInitialDestinationIndex() const
{
  return mInitialDestinationIndex;
}

template <typename T>
void JamomaCurve<T>::setInitialDestinationIndex(std::initializer_list<char> index)
{
  mInitialDestinationIndex.clear();
  
  for (const auto & i : index)
    mInitialDestinationIndex.push_back(i);
}

template <typename T>
map<const TimeValue, pair<T, shared_ptr<CurveSegment<T>>>> JamomaCurve<T>::getPointsMap() const
{
  return mPointsMap;
}

# pragma mark -
# pragma mark Implementation specific

template <typename T>
T JamomaCurve<T>::convertToTemplateTypeValue(const Value * value, char* level) const
{
  switch (value->getType())
  {
    case Value::Type::BOOL :
    {
      auto b = static_cast<const Bool*>(value);
      return b->value;
    }
      
    case Value::Type::INT :
    {
      auto i = static_cast<const Int*>(value);
      return i->value;
    }
      
    case Value::Type::FLOAT :
    {
      auto f = static_cast<const Float*>(value);
      return f->value;
    }
      
    case Value::Type::CHAR :
    {
      auto c = static_cast<const Char*>(value);
      return c->value;
    }
    
    case Value::Type::TUPLE :
    {
      auto t = static_cast<const Tuple*>(value);
      
      char index = mInitialDestinationIndex[*level];
      (*level)++;

      return convertToTemplateTypeValue(t->value[index], level);
    }
      
    case Value::Type::GENERIC :
    {
      //! \todo GENERIC case
    }
      
    default :
    {
      throw runtime_error("converting none numerical value");
    }
  }
}