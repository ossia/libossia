/*!
 * \file CurveSegmentLinear.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef CURVESEGMENTLINEAR_H_
#define CURVESEGMENTLINEAR_H_

#include "Editor/CurveSegment.h"

namespace OSSIA {
  
  template <typename T>
  class CurveSegmentLinear : public CurveSegment<T> {
    
  public:
    
    // Constructors, destructor, assignment
    CurveSegmentLinear(Curve<T>*);
    CurveSegmentLinear(const CurveSegmentLinear&);
    virtual ~CurveSegmentLinear();
    CurveSegmentLinear & operator= (const CurveSegmentLinear&);
    
    // Computation
    virtual T valueAt(double) const override;
    
    // Curve segment type
    virtual typename CurveSegment<T>::CurveSegmentType getType() const override final
    {return CurveSegment<T>::CurveSegmentType::LINEAR;};
    
    // pimpl idiom
  private:
    class Impl;
    Impl * pimpl{};
    
  };

}

#endif /* CURVESEGMENTLINEAR_H_ */
