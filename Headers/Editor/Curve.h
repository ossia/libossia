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

template <typename T>
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
  const_iterator find(double) const;

  // Manage points (abscissa in double between 0 and 1)
  bool addPoint(double, const T, const CurveSegment<T>&);
  bool removePoint(double);

  // Accessors
  T getInitialValue() const;
  void setInitialValue(const T);
  std::map<double, std::pair<T, CurveSegment<T>&>> getPointsMap() const;
    // {abscissa, {value, previous segment}}
  
  // Computation
  T valueAt(double) const; // Between 0. and 1.

  // pimpl idiom
// private: // todo issue #1 : 
  class Impl;
  Impl * pimpl;

};
  
  // explicit instantiation for double
  template class Curve<double>;

}

#endif /* CURVE_H_ */
