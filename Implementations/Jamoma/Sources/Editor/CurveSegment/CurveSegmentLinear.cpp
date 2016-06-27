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

#include <iostream> //! \todo to remove. only here for debug purpose

# pragma mark -
# pragma mark Life Cycle

template <typename Y>
JamomaCurveSegmentLinear<Y>::
JamomaCurveSegmentLinear(shared_ptr<CurveAbstract> parent) :
mParent(parent)
{}

template <typename Y>
JamomaCurveSegmentLinear<Y>::
JamomaCurveSegmentLinear(const JamomaCurveSegmentLinear * other)
{}

template <typename Y>
shared_ptr<CurveSegmentLinear<Y>> JamomaCurveSegmentLinear<Y>::
clone() const
{
    return make_shared<JamomaCurveSegmentLinear<Y>>(this);
}

template <typename Y>
JamomaCurveSegmentLinear<Y>::
~JamomaCurveSegmentLinear()
{}

# pragma mark -
# pragma mark Execution

template <typename Y>
Y JamomaCurveSegmentLinear<Y>::
valueAt(double ratio, Y start, Y end) const
{
  return start + ratio * (end - start);
}

# pragma mark -
# pragma mark Accessors

template <typename Y>
shared_ptr<CurveAbstract> JamomaCurveSegmentLinear<Y>::
getParent() const
{
  return mParent;
}


namespace OSSIA
{
  template <typename Y>
  shared_ptr<CurveSegmentLinear<Y>> CurveSegmentLinear<Y>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentLinear<Y>>(parent);
  }

  template <typename Y>
  CurveSegmentLinear<Y>::~CurveSegmentLinear()
  {

  }
}
// Explicit instantiation
template class OSSIA_EXPORT OSSIA::CurveSegmentLinear<bool>;
template class OSSIA_EXPORT OSSIA::CurveSegmentLinear<int>;
template class OSSIA_EXPORT OSSIA::CurveSegmentLinear<float>;
