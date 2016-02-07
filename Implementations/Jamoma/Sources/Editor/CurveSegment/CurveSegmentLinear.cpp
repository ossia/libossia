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

namespace OSSIA
{
  // explicit instantiation for bool
  template class CurveSegmentLinear<bool>;
  
  template <>
  shared_ptr<CurveSegmentLinear<bool>> CurveSegmentLinear<bool>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentLinear<bool>>(parent);
  }
  
  template <>
  CurveSegmentLinear<bool>::~CurveSegmentLinear()
  {}
  
  // explicit instantiation for int
  template class CurveSegmentLinear<int>;
  
  template <>
  shared_ptr<CurveSegmentLinear<int>> CurveSegmentLinear<int>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentLinear<int>>(parent);
  }
  
  template <>
  CurveSegmentLinear<int>::~CurveSegmentLinear()
  {}
  
  // explicit instantiation for float
  template class CurveSegmentLinear<float>;
  
  template <>
  shared_ptr<CurveSegmentLinear<float>> CurveSegmentLinear<float>::create(shared_ptr<CurveAbstract> parent)
  {
    return make_shared<JamomaCurveSegmentLinear<float>>(parent);
  }
  
  template <>
  CurveSegmentLinear<float>::~CurveSegmentLinear()
  {}
}

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

template <typename Y>
CurveSegmentLinear<Y>::~CurveSegmentLinear()
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
