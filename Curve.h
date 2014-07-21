/*!
 * \file Curve.h
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef CURVE_H_
#define CURVE_H_

#include <map>
#include <utility>

namespace OSSIA {

class CurveSegment;

template <typename T>
class Curve {

public:

  typedef T value_type;

  // Constructors, destructor, assignment
  Curve();
  Curve(const Curve&);
  ~Curve();
  Curve & operator= (const Curve&);

  // Iterators
  class const_iterator; // bidirectional
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator find(const double) const;

  // Manage points (abscissa in double between 0 and 1)
  bool addPoint(const double, const T, const CurveSegment&);
  bool removePoint(const double);

  // Accessors
  T getInitialValue() const;
  void setInitialValue(const T);
  std::map<double, std::pair<T, CurveSegment&>> getPointsMap() const;

  // pimpl idiom
private:
  class Impl;
  Impl * pimpl;

};

}





#endif /* CURVE_H_ */
