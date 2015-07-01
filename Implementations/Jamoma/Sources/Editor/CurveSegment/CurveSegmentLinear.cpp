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
  // explicit instantiation for double
  template class CurveSegmentLinear<double>;
  
  template <>
  shared_ptr<CurveSegmentLinear<double>> CurveSegmentLinear<double>::create(shared_ptr<Curve<double>> parent)
  {
    return make_shared<JamomaCurveSegmentLinear<double>>(parent);
  }
}

template <typename T>
JamomaCurveSegmentLinear<T>::JamomaCurveSegmentLinear(shared_ptr<Curve<T>> parent) :
mParent(parent)
{}

template <typename T>
JamomaCurveSegmentLinear<T>::~JamomaCurveSegmentLinear()
{}

# pragma mark -
# pragma mark Execution

template <typename T>
T JamomaCurveSegmentLinear<T>::valueAt(double abscissa, T target) const
{
  return abscissa * target;
}

# pragma mark -
# pragma mark Accessors

template <typename T>
shared_ptr<Curve<T>> JamomaCurveSegmentLinear<T>::getParent() const
{
  return mParent;
}