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

namespace OSSIA {
  
  template <typename T>
  class Curve;
  
  template <typename T>
  class CurveSegment {
    
  public:
    
    // Constructor, Destructor
    CurveSegment() : mParent(nullptr) {};
    CurveSegment(Curve<T> * parent) : mParent(parent) {};
    virtual ~CurveSegment() {};
    
    // Navigation
    Curve<T>* getParent() const {return mParent;};
    
    // Computation
    virtual T valueAt(double) const // Between 0. and 1.
    {
      T t = 0.;
      return t;
    };
    
    // Curve segment types
    enum class CurveSegmentType {
      NONE,
      LINEAR,
      POWER
    };
    
    virtual CurveSegmentType getType() const
    {return CurveSegment<T>::CurveSegmentType::NONE;};
    
  protected:
    
    Curve<T>* mParent{};
    
  };
  
}

#endif /* CURVESEGMENT_H_ */
