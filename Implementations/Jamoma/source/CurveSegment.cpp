/*!
 * \file CurveSegment.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/CurveSegment.h"

namespace OSSIA {
  
  template <typename T>
  class CurveSegment<T>::Impl {
    
  public:
    
    Impl() {};
    
    Impl(const Impl & other) = default;
    ~Impl() = default;
    
    CurveSegmentType getType() {return NONE_TYPE;};
    
    // Computation
    T valueAt(double abscissa)
    {return 0;}
    
  };
  
  template <typename T>
  CurveSegment<T>::CurveSegment() :
  pimpl(new Impl)
  {}
  
  template <typename T>
  CurveSegment<T>::CurveSegment(const CurveSegment & other) :
  pimpl(new Impl(other.pimpl))
  {}
  
  template <typename T>
  CurveSegment<T>::~CurveSegment()
  {
    delete pimpl;
  }
  
  template <typename T>
  CurveSegment<T>& CurveSegment<T>::operator= (const CurveSegment & other)
  {
    delete pimpl;
    pimpl = new Impl(other.pimpl);
  }
}
