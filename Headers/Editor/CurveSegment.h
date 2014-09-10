/*!
 * \file CurveSegment.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef CURVESEGMENT_H_
#define CURVESEGMENT_H_

#include "Editor/Curve.h" // inclusion instead of forward declaration for explicit instantiation purpose

namespace OSSIA {
  
  template <typename T>
  class CurveSegment {
    
  public:
    
    // Constructor, Destructor
    CurveSegment(Curve<T> & parent) : mParent(parent) {};
    virtual ~CurveSegment() {};
    
    // Navigation
    Curve<T>& getParent() const {return mParent;};
    
    // Computation
    virtual T valueAt(double) const = 0; // Between 0. and 1.
    
    // Curve segment types
    enum CurveSegmentType {
      NONE_TYPE,
      LINEAR_TYPE,
      POWER_TYPE
    };
    
    virtual CurveSegmentType getType() const = 0;
    
  protected:
    
    Curve<T>& mParent;
    
  };
  
  // explicit instantiation for double
  template class CurveSegment<double>;
  
}

#endif /* CURVESEGMENT_H_ */
