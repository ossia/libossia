/*!
 * \file CurveSegment.h
 *
 * \defgroup Editor
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

class CurveAbstract;

template <typename Y>
class CurveSegment
{

public:
  
# pragma mark -
# pragma mark Enumerations
  
  /*! type of segment */
  enum class Type
  {
    EMPTY,
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
   \param double ratio between 0. and 1.
   \param Y segment start value
   \param Y segment end value
   \return Y value */
  virtual Y valueAt(double, Y, Y) const = 0;

# pragma mark -
# pragma mark Accessors
  
  /* get the type of the segment 
   \return #Type */
  virtual Type getType() const = 0;
  
  /*! get the curve where the segment is
   \return std::shared_ptr<CurveAbstract>> */
  virtual std::shared_ptr<CurveAbstract> getParent() const = 0;
  
};
}

