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
class CurveSegment {

public:

  // Destructor only
  virtual ~CurveSegment() = 0;
  
  // Computation
  virtual T valueAt(double) const = 0; // Between 0 and 1
  
  // Curve segment types
  enum CurveSegmentType {
    NONE_TYPE,
    LINEAR_TYPE,
    POWER_TYPE
  };
    
  virtual CurveSegmentType getType() const = 0;

};

}



#endif /* CURVESEGMENT_H_ */
