/*!
 * \file CurveSegment.h
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
#include <memory>

namespace OSSIA
{

template <typename T>
class Curve;

template <typename T>
class CurveSegment
{

public:
  
# pragma mark -
# pragma mark Enumerations
  
  /*! type of segment */
  enum class Type
  {
    IMPULSE,
    LINEAR,
    POWER
  };

# pragma mark -
# pragma mark Life cycle
  
  /*! destructor */
  virtual ~CurveSegment() = default;
 
# pragma mark -
# pragma mark Execution
  
  /*! get value at an abscissa
   \param const #TimeValue ratio between 0. and 1.
   \param T segment start value
   \param T segment end value
   \return T value */
  virtual T valueAt(const TimeValue&, T, T) const = 0;

# pragma mark -
# pragma mark Accessors
  
  /* get the type of the segment 
   \return #Type */
  virtual Type getType() const = 0;
  
  /*! get the curve where the segment is
   \return std::shared_ptr<Curve<T>> */
  virtual std::shared_ptr<Curve<T>> getParent() const = 0;
  
};
}

