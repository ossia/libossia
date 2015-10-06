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
{}

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
  T lastValue = mInitialValue;
  
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
  return mInitialValue;
}

template <typename T>
void JamomaCurve<T>::setInitialValue(const T value)
{
  mInitialValue = value;
}

template <typename T>
map<const TimeValue, pair<T, shared_ptr<CurveSegment<T>>>> JamomaCurve<T>::getPointsMap() const
{
  return mPointsMap;
}