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
  shared_ptr<CurveSegmentPower<bool>> CurveSegmentPower<bool>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentPower<bool>>(parent);
  }
  
  // explicit instantiation for int
  template class CurveSegmentPower<int>;
  
  template <>
  shared_ptr<CurveSegmentPower<int>> CurveSegmentPower<int>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentPower<int>>(parent);
  }
  
  // explicit instantiation for float
  template class CurveSegmentPower<float>;
  
  template <>
  shared_ptr<CurveSegmentPower<float>> CurveSegmentPower<float>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentPower<float>>(parent);
  }
}

template <typename Y>
JamomaCurveSegmentPower<Y>::
JamomaCurveSegmentPower(shared_ptr<CurveAbstract> parent) :
mParent(parent),
mPower(1.)
{}

template <typename Y>
JamomaCurveSegmentPower<Y>::
JamomaCurveSegmentPower(const JamomaCurveSegmentPower * other) :
mParent(other->mParent),
mPower(other->mPower)
{}

template <typename Y>
shared_ptr<CurveSegmentPower<Y>> JamomaCurveSegmentPower<Y>::
clone() const
{
  return make_shared<JamomaCurveSegmentPower<Y>>(this);
}

template <typename Y>
JamomaCurveSegmentPower<Y>::
~JamomaCurveSegmentPower()
{}

# pragma mark -
# pragma mark Execution

template <typename Y>
Y JamomaCurveSegmentPower<Y>::
valueAt(double ratio, Y start, Y end) const
{
  return start + pow(ratio, mPower) * (end - start);
}

# pragma mark -
# pragma mark Accessors

template <typename Y>
shared_ptr<CurveAbstract> JamomaCurveSegmentPower<Y>::
getParent() const
{
  return mParent;
}

template <typename Y>
double JamomaCurveSegmentPower<Y>::
getPower() const
{
  return mPower;
}

template <typename Y>
CurveSegmentPower<Y> & JamomaCurveSegmentPower<Y>::
setPower(double power)
{
  mPower = power;
  return *this;
}
