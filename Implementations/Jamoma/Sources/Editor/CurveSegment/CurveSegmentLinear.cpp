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
T JamomaCurveSegmentLinear<T>::valueAt(double abscissa) const
{
  /*
  double previousAbscissa = 0.;

  // get the previous point abscissa to add it to the given abscissa
  auto pointsMap = mParent->getPointsMap();
  
  for (auto it = pointsMap.begin(); it != pointsMap.end(); it++)
  {
    // when this curve is found
    if (&(it->second.second) == dynamic_cast<const CurveSegment<T>*>(this))
    {
      // get the previous point abscissa
      if (it != pointsMap.begin())
      {
        it--;
        previousAbscissa = it->first;
      }
      
      break;
    }
  }
  
  return mParent->valueAt(previousAbscissa + abscissa);
   */
}

# pragma mark -
# pragma mark Accessors

template <typename T>
shared_ptr<Curve<T>> JamomaCurveSegmentLinear<T>::getParent() const
{
  return mParent;
}