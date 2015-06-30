/*!
 * \file CurveSegmentLinear.h
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

#include "Editor/CurveSegment.h"

namespace OSSIA
{

template <typename T>
class CurveSegmentLinear : public CurveSegment<T>
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! constructor */
  CurveSegmentLinear(Curve<T>*);
  
  /*! destructor */
  virtual ~CurveSegmentLinear();

# pragma mark -
# pragma mark Execution
  
  virtual T valueAt(double) const override;

# pragma mark -
# pragma mark Accessors

  typename CurveSegment<T>::Type getType() const override final
  {return CurveSegment<T>::Type::LINEAR;}

};
}
