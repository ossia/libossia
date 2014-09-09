/*!
 * \file CurveSegmentNone.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef CURVESEGMENTNONE_H_
#define CURVESEGMENTNONE_H_

#include "Editor/CurveSegment.h"

namespace OSSIA {
  
  template <typename T>
  class CurveSegmentNone : public CurveSegment<T> {
    
  public:
    
    // Constructors, destructor, assignment
    CurveSegmentNone(Curve<T>&);
    CurveSegmentNone(const CurveSegmentNone&);
    virtual ~CurveSegmentNone();
    CurveSegmentNone & operator= (const CurveSegmentNone&);
    
    // Computation
    virtual T valueAt(double) const override final
    {return;};
    
    // Curve segment type
    virtual CurveSegmentType getType() const override final
    {return NONE_TYPE;};
    
    // pimpl idiom
  private:
    class Impl;
    Impl * pimpl;
    
  };
  
}



#endif /* CURVESEGMENTNONE_H_ */
