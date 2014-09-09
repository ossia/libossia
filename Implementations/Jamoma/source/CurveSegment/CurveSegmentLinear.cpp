/*!
 * \file CurveSegmentLinear.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/CurveSegment/CurveSegmentLinear.h"

namespace OSSIA {
  
  template <typename T>
  class CurveSegmentLinear<T>::Impl {
    
  public:
    
    //TTObject mParentCurve;
    
    Impl() /*: mParentCurve(parentCurve)*/ {};
    
    Impl(const Impl & other) = default;
    ~Impl() = default;
    
  };
  
  template <typename T>
  CurveSegmentLinear<T>::CurveSegmentLinear(Curve<T> & parent) :
  CurveSegment<T>(parent),
  pimpl(new Impl)
  {}
  
  template <typename T>
  CurveSegmentLinear<T>::CurveSegmentLinear(const CurveSegmentLinear & other) :
  CurveSegment<T>(other.getParent()),
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  template <typename T>
  CurveSegmentLinear<T>::~CurveSegmentLinear()
  {
    delete pimpl;
  }
  
  template <typename T>
  CurveSegmentLinear<T>& CurveSegmentLinear<T>::operator= (const CurveSegmentLinear & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }

  template <typename T>
  T CurveSegmentLinear<T>::valueAt(double abscissa) const
  {
    // TODO
    return 0.;
  }

}