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

template <typename T>
class Curve;

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
    virtual T valueAt(double) const = 0; // Between 0 and 1
    
    // Curve segment types
    enum CurveSegmentType {
      NONE_TYPE,
      LINEAR_TYPE,
      POWER_TYPE
    };
    
    virtual CurveSegmentType getType() const = 0;
    
  private:
    
    Curve<T>& mParent;
    
  };
  
}



#endif /* CURVESEGMENT_H_ */
