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
    {
      T t;
      return t;
    };
    
    // Curve segment type
    virtual typename CurveSegment<T>::CurveSegmentType getType() const override final
    {return CurveSegment<T>::NONE_TYPE;};
    
    // pimpl idiom
  private:
    class Impl;
    Impl * pimpl;
    
  };
  
  // explicit instantiation for double
  template class CurveSegmentNone<double>;
}

#endif /* CURVESEGMENTNONE_H_ */
