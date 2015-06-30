#include "Editor/JamomaCurve.h"

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
T JamomaCurve<T>::valueAt(double) const
{
  
}

# pragma mark -
# pragma mark Accessors

template <typename T>
T JamomaCurve<T>::getInitialValue() const
{
  
}

template <typename T>
void JamomaCurve<T>::setInitialValue(const T)
{
  
}

template <typename T>
map<double, pair<T, shared_ptr<CurveSegment<T>>>> JamomaCurve<T>::getPointsMap() const
{
  return mMap;
}

# pragma mark -
# pragma mark CurveSegments

template <typename T>
bool JamomaCurve<T>::addPoint(double, T, shared_ptr<CurveSegment<T>>)
{
  return false;
}

template <typename T>
bool JamomaCurve<T>::removePoint(double)
{
  return false;
}