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

# pragma mark -
# pragma mark Life Cycle

namespace OSSIA
{
  // explicit instantiation for bool
  template class CurveSegmentEmpty<bool>;
  
  template <>
  shared_ptr<CurveSegmentEmpty<bool>> CurveSegmentEmpty<bool>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentEmpty<bool>>(parent);
  }
  
  // explicit instantiation for int
  template class CurveSegmentEmpty<int>;
  
  template <>
  shared_ptr<CurveSegmentEmpty<int>> CurveSegmentEmpty<int>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentEmpty<int>>(parent);
  }
  
  // explicit instantiation for float
  template class CurveSegmentEmpty<float>;
  
  template <>
  shared_ptr<CurveSegmentEmpty<float>> CurveSegmentEmpty<float>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentEmpty<float>>(parent);
  }
}

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

# pragma mark -
# pragma mark Execution

template <typename Y>
Y JamomaCurveSegmentEmpty<Y>::
valueAt(double ratio, Y start, Y end) const
{
  return start;
}

# pragma mark -
# pragma mark Accessors

template <typename Y>
shared_ptr<CurveAbstract> JamomaCurveSegmentEmpty<Y>::
getParent() const
{
  return mParent;
}
