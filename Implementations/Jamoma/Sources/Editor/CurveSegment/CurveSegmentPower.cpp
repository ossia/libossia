/*!
 * \file CurveSegmentPower.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/CurveSegment/CurveSegmentPower.h"

#include "TTCurve.h"

#include "../Implementations/Jamoma/Sources/Editor/Curve.cpp" // because we use the parent curve into the segment (see : valueAt)

namespace OSSIA {
  
template <typename T>
class CurveSegmentPower<T>::Impl {
  
public:
  
  Impl() {};
  
  Impl(const Impl & other) = default;
  ~Impl() = default;
  
};

template <typename T>
CurveSegmentPower<T>::CurveSegmentPower(Curve<T> * parent) :
CurveSegment<T>(parent),
pimpl(new Impl)
{}

template <typename T>
CurveSegmentPower<T>::CurveSegmentPower(const CurveSegmentPower & other) :
CurveSegment<T>(other.getParent()),
pimpl(new Impl(*(other.pimpl)))
{}

template <typename T>
CurveSegmentPower<T>::~CurveSegmentPower()
{
  delete pimpl;
}

template <typename T>
CurveSegmentPower<T>& CurveSegmentPower<T>::operator= (const CurveSegmentPower & other)
{
  delete pimpl;
  pimpl = new Impl(*(other.pimpl));
  return *this;
}

template <typename T>
T CurveSegmentPower<T>::valueAt(double abscissa) const
{
  TTValue   out;
  TTFloat64 previousAbscissa = 0.;

  // get the previous point abscissa to add it to the given abscissa
  auto pointsMap = CurveSegment<T>::mParent->getPointsMap();
  
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
  
  return CurveSegment<T>::mParent->valueAt(previousAbscissa + abscissa);
}
  
  // explicit instantiation for double
  template class CurveSegmentPower<double>;

}