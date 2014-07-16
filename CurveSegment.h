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

  // Constructors, destructor, assignment
  CurveSegment();
  CurveSegment(const CurveSegment&);
  ~CurveSegment();
  CurveSegment & operator= (const CurveSegment&);

  // Computation
  T valueAt(TimeValue) const;

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}



#endif /* CURVESEGMENT_H_ */
