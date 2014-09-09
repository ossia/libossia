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
  CurveSegmentLinear(Curve<T> & parent);
  CurveSegmentLinear(const CurveSegmentLinear&);
  virtual ~CurveSegmentLinear();
  CurveSegmentLinear & operator= (const CurveSegmentLinear&);
    
  virtual CurveSegmentType getType() const override final
    {return LINEAR_TYPE;};

  // Computation
  virtual T valueAt(double) const override;

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}



#endif /* CURVESEGMENTLINEAR_H_ */
