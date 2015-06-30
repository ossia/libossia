/*!
 * \file Curve.h
 *
 * \brief
 *
 * \details
 *
 * \author Clément Bossut
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include <map>
#include <utility>
#include <memory>

#include "Misc/Container.h"

namespace OSSIA
{

template <typename T>
class CurveSegment;

template <typename T>
class Curve
{

public:
  
  typedef T value_type;
  
# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \return std::shared_ptr<#Curve> */
  static std::shared_ptr<Curve> create();
  
  /*! clone */
  virtual std::shared_ptr<Curve> clone() const = 0;
  
  /*! destructor */
  virtual ~Curve() = default;

# pragma mark -
# pragma mark Execution
  
  /*! get value at an abscissa
   \param double abscissa between 0. and 1.
   \return T value */
  virtual T valueAt(double) const = 0;

# pragma mark -
# pragma mark Accessors
  
  /*! get initial curve value
   \return T value */
  virtual T getInitialValue() const = 0;
  
  /*! set initial curve value
   \param const T value */
  virtual void setInitialValue(const T) = 0;
  
  /*! get initial curve value
   \return std::map<double, std::pair<T, std::shared_ptr<CurveSegment<T>>>> map of {abscissa, {value, previous segment} */
  virtual std::map<double, std::pair<T, std::shared_ptr<CurveSegment<T>>>> getPointsMap() const = 0;

# pragma mark -
# pragma mark CurveSegments
  
  /*! add a segment to the curve
   \param double abscissa between 0. and 1.
   \param T initial segment value
   \param #CurveSegment<T> segment
   \return bool */
  virtual bool addSegment(double, T, CurveSegment<T>&) = 0;
  
  /*! remove a segment from the curve
   \param double abscissa between 0. and 1.
   \return bool */
  virtual bool removeSegment(double) = 0;

  /*! get curve segments of the curve
   \return #Container<#CurveSegment<T>> */
  Container<CurveSegment<T>>& curveSegments()
  { return m_curveSegments; }
  
  /*! get curve segments of the curve
   \return const #Container<#CurveSegment<T>> */
  const Container<CurveSegment<T>>& curveSegments() const
  { return m_curveSegments; }

private:
  Container<CurveSegment<T>> m_curveSegments;
};

}
