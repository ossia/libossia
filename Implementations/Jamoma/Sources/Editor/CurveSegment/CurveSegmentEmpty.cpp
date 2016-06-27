/*!
 * \file CurveSegmentEmpty.cpp
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/JamomaCurve.h"
#include "Editor/CurveSegment/JamomaCurveSegmentEmpty.h"
#if 0
# pragma mark -
# pragma mark Life Cycle
#endif

template <typename Y>
JamomaCurveSegmentEmpty<Y>::
JamomaCurveSegmentEmpty(shared_ptr<CurveAbstract> parent) :
mParent(parent)
{}

template <typename Y>
JamomaCurveSegmentEmpty<Y>::
JamomaCurveSegmentEmpty(const JamomaCurveSegmentEmpty * other)
{}

template <typename Y>
shared_ptr<CurveSegmentEmpty<Y>> JamomaCurveSegmentEmpty<Y>::
clone() const
{
    return make_shared<JamomaCurveSegmentEmpty<Y>>(this);
}

template <typename Y>
JamomaCurveSegmentEmpty<Y>::
~JamomaCurveSegmentEmpty()
{}

#if 0
# pragma mark -
# pragma mark Execution
#endif

template <typename Y>
Y JamomaCurveSegmentEmpty<Y>::
valueAt(double ratio, Y start, Y end) const
{
  return start;
}
#if 0
# pragma mark -
# pragma mark Accessors
#endif
template <typename Y>
shared_ptr<CurveAbstract> JamomaCurveSegmentEmpty<Y>::
getParent() const
{
  return mParent;
}


namespace OSSIA
{
  template <typename Y>
  shared_ptr<CurveSegmentEmpty<Y>> CurveSegmentEmpty<Y>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentEmpty<Y>>(parent);
  }

  template <typename Y>
  CurveSegmentEmpty<Y>::~CurveSegmentEmpty()
  {

  }
}
// Explicit instantiation
template class OSSIA_EXPORT OSSIA::CurveSegmentEmpty<bool>;
template class OSSIA_EXPORT OSSIA::CurveSegmentEmpty<int>;
template class OSSIA_EXPORT OSSIA::CurveSegmentEmpty<float>;
