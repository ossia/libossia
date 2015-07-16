/*!
 * \file CurveSegmentPower.cpp
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
#include "Editor/CurveSegment/JamomaCurveSegmentPower.h"

# pragma mark -
# pragma mark Life Cycle

namespace OSSIA
{
  // explicit instantiation for bool
  template class CurveSegmentPower<bool>;
  
  template <>
  shared_ptr<CurveSegmentPower<bool>> CurveSegmentPower<bool>::create(shared_ptr<Curve<bool>> parent)
  {
    return make_shared<JamomaCurveSegmentPower<bool>>(parent);
  }
  
  // explicit instantiation for int
  template class CurveSegmentPower<int>;
  
  template <>
  shared_ptr<CurveSegmentPower<int>> CurveSegmentPower<int>::create(shared_ptr<Curve<int>> parent)
  {
    return make_shared<JamomaCurveSegmentPower<int>>(parent);
  }
  
  // explicit instantiation for float
  template class CurveSegmentPower<float>;
  
  template <>
  shared_ptr<CurveSegmentPower<float>> CurveSegmentPower<float>::create(shared_ptr<Curve<float>> parent)
  {
    return make_shared<JamomaCurveSegmentPower<float>>(parent);
  }
}

template <typename T>
JamomaCurveSegmentPower<T>::JamomaCurveSegmentPower(shared_ptr<Curve<T>> parent) :
mParent(parent),
mPower(1.)
{}

template <typename T>
JamomaCurveSegmentPower<T>::JamomaCurveSegmentPower(const JamomaCurveSegmentPower * other) :
mParent(other->mParent),
mPower(other->mPower)
{}

template <typename T>
shared_ptr<CurveSegmentPower<T>> JamomaCurveSegmentPower<T>::clone() const
{
  return make_shared<JamomaCurveSegmentPower<T>>(this);
}

template <typename T>
JamomaCurveSegmentPower<T>::~JamomaCurveSegmentPower()
{}

# pragma mark -
# pragma mark Execution

template <typename T>
T JamomaCurveSegmentPower<T>::valueAt(const TimeValue& ratio, T start, T end) const
{
  return start + pow(ratio, mPower) * (end - start);
}

# pragma mark -
# pragma mark Accessors

template <typename T>
shared_ptr<Curve<T>> JamomaCurveSegmentPower<T>::getParent() const
{
  return mParent;
}

template <typename T>
double JamomaCurveSegmentPower<T>::getPower() const
{
  return mPower;
}

template <typename T>
CurveSegmentPower<T> & JamomaCurveSegmentPower<T>::setPower(double power)
{
  mPower = power;
  return *this;
}
