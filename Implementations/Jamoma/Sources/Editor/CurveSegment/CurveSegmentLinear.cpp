/*!
 * \file CurveSegmentLinear.cpp
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/JamomaCurve.h"
#include "Editor/CurveSegment/JamomaCurveSegmentLinear.h"

# pragma mark -
# pragma mark Life Cycle

namespace OSSIA
{
  // explicit instantiation for bool
  template class CurveSegmentLinear<bool>;
  
  template <>
  shared_ptr<CurveSegmentLinear<bool>> CurveSegmentLinear<bool>::create(shared_ptr<Curve<bool>> parent)
  {
    return make_shared<JamomaCurveSegmentLinear<bool>>(parent);
  }
  
  // explicit instantiation for int
  template class CurveSegmentLinear<int>;
  
  template <>
  shared_ptr<CurveSegmentLinear<int>> CurveSegmentLinear<int>::create(shared_ptr<Curve<int>> parent)
  {
    return make_shared<JamomaCurveSegmentLinear<int>>(parent);
  }
  
  // explicit instantiation for float
  template class CurveSegmentLinear<float>;
  
  template <>
  shared_ptr<CurveSegmentLinear<float>> CurveSegmentLinear<float>::create(shared_ptr<Curve<float>> parent)
  {
    return make_shared<JamomaCurveSegmentLinear<float>>(parent);
  }
}

template <typename T>
JamomaCurveSegmentLinear<T>::JamomaCurveSegmentLinear(shared_ptr<Curve<T>> parent) :
mParent(parent)
{}

template <typename T>
JamomaCurveSegmentLinear<T>::JamomaCurveSegmentLinear(const JamomaCurveSegmentLinear * other)
{}

template <typename T>
shared_ptr<CurveSegmentLinear<T>> JamomaCurveSegmentLinear<T>::clone() const
{
    return make_shared<JamomaCurveSegmentLinear<T>>(this);
}

template <typename T>
JamomaCurveSegmentLinear<T>::~JamomaCurveSegmentLinear()
{}

# pragma mark -
# pragma mark Execution

template <typename T>
T JamomaCurveSegmentLinear<T>::valueAt(const TimeValue& ratio, T start, T end) const
{
  return start + ratio * (end - start);
}

# pragma mark -
# pragma mark Accessors

template <typename T>
shared_ptr<Curve<T>> JamomaCurveSegmentLinear<T>::getParent() const
{
  return mParent;
}
