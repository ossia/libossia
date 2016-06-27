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


namespace OSSIA
{
  template <typename Y>
  std::shared_ptr<OSSIA::CurveSegmentPower<Y>> CurveSegmentPower<Y>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentPower<Y>>(parent);
  }

  template <typename Y>
  CurveSegmentPower<Y>::~CurveSegmentPower()
  {

  }
}
// Explicit instantiation
template class OSSIA_EXPORT OSSIA::CurveSegmentPower<bool>;
template class OSSIA_EXPORT OSSIA::CurveSegmentPower<int>;
template class OSSIA_EXPORT OSSIA::CurveSegmentPower<float>;
