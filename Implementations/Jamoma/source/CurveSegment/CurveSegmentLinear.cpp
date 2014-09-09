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
  
  class CurveSegmentLinear::Impl {
    
  public:
    
    Impl() {};
    
    Impl(const Impl & other) = default;
    ~Impl() = default;
    
  };
  
  CurveSegmentLinear::CurveSegmentLinear() :
  pimpl(new Impl)
  {}
  
  CurveSegmentLinear::CurveSegmentLinear(const CurveSegmentLinear & other) :
  pimpl(new Impl(*(other.pimpl)))
  {}
  
  CurveSegmentLinear::~CurveSegmentLinear()
  {
    delete pimpl;
  }
  
  CurveSegmentLinear& CurveSegmentLinear::operator= (const CurveSegmentLinear & other)
  {
    delete pimpl;
    pimpl = new Impl(*(other.pimpl));
    return *this;
  }

  double CurveSegmentLinear::valueAt(double abscissa) const
  {
    // TODO
    return 0.;
  }

}