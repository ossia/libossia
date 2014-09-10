/*!
 * \file CurveSegmentNone.cpp
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Editor/CurveSegment/CurveSegmentNone.h"

namespace OSSIA {
  
  template <typename T>
  class CurveSegmentNone<T>::Impl {
    
  public:
    
    Impl() {};
    
    Impl(const Impl & other) = default;
    ~Impl() = default;
    
  };
  
  template <typename T>
  CurveSegmentNone<T>::CurveSegmentNone(Curve<T> & parent) :
  CurveSegment<T>(parent),
  pimpl(new Impl)
  {}
  
  template <typename T>
  CurveSegmentNone<T>::CurveSegmentNone(const CurveSegmentNone & other) :
  CurveSegment<T>(other.getParent()),
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  template <typename T>
  CurveSegmentNone<T>::~CurveSegmentNone()
  {
    delete pimpl;
  }
  
  template <typename T>
  CurveSegmentNone<T>& CurveSegmentNone<T>::operator= (const CurveSegmentNone & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }

}