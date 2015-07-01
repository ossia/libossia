#include "Editor/JamomaCurve.h"
#include "Editor/CurveSegment/JamomaCurveSegmentLinear.h"

#include <iostream> //! \todo to remove. only here for debug purpose

using namespace OSSIA;
using namespace std;

# pragma mark -
# pragma mark Life cycle

namespace OSSIA
{
  // explicit instantiation for double
  template class Curve<double>;
  
  template <>
  shared_ptr<Curve<double>> Curve<double>::create()
  {
    return make_shared<JamomaCurve<double>>();
  }
}

template <typename T>
JamomaCurve<T>::JamomaCurve()
{}

template <typename T>
JamomaCurve<T>::JamomaCurve(const JamomaCurve * other)
{}

template <typename T>
JamomaCurve<T>::~JamomaCurve()
{}

template <typename T>
shared_ptr<Curve<T>> JamomaCurve<T>::clone() const
{
  return make_shared<JamomaCurve>(this);
}

# pragma mark -
# pragma mark Execution

template <typename T>
T JamomaCurve<T>::valueAt(double abscissa) const
{
  double lastAbscissa = 0.;
  T lastValue = mInitialValue;
  
  for (auto it = mPointsMap.begin(); it != mPointsMap.end(); it++)
  {
    if (abscissa > lastAbscissa &&
        abscissa <= it->first)
    {
      double segmentAbscissa = (abscissa - lastAbscissa) / (it->first - lastAbscissa);
      T segmentDistance = it->second.first - lastValue;
      
      lastValue += it->second.second->valueAt(segmentAbscissa, segmentDistance);
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
map<double, pair<T, shared_ptr<CurveSegment<T>>>> JamomaCurve<T>::getPointsMap() const
{
  return mPointsMap;
}

# pragma mark -
# pragma mark CurveSegments

template <typename T>
bool JamomaCurve<T>::addPoint(double abscissa, T value, shared_ptr<CurveSegment<T>> segment)
{
  pair<T,shared_ptr<CurveSegment<T>>> p(value, segment);
  
  //! \todo check if there is already a point
  
  mPointsMap.emplace(abscissa, p);
  
  return true;
}

template <typename T>
bool JamomaCurve<T>::removePoint(double abscissa)
{
  return mPointsMap.erase(abscissa) > 0;
}