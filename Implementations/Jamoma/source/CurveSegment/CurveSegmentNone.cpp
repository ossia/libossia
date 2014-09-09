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
  
  class CurveSegmentNone::Impl {
    
  public:
    
    Impl() {};
    
    Impl(const Impl & other) = default;
    ~Impl() = default;
    
  };
  
  CurveSegmentNone::CurveSegmentNone() :
  pimpl(new Impl)
  {}
  
  CurveSegmentNone::CurveSegmentNone(const CurveSegmentNone & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  CurveSegmentNone::~CurveSegmentNone()
  {
    delete pimpl;
  }
  
  CurveSegmentNone& CurveSegmentNone::operator= (const CurveSegmentNone & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }
}