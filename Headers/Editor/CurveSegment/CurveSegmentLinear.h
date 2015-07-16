/*!
 * \file CurveSegmentLinear.h
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

#include "Editor/CurveSegment.h"

namespace OSSIA
{

template <typename T>
class CurveSegmentLinear : public CurveSegment<T>
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param std::shared_ptr<Curve<T>> parent
   \return std::shared_ptr<CurveSegmentLinear<T>> */
  static std::shared_ptr<CurveSegmentLinear<T>> create(std::shared_ptr<Curve<T>> = nullptr);

  /*! clone */
  virtual std::shared_ptr<CurveSegmentLinear<T>> clone() const = 0;
  
  /*! destructor */
  virtual ~CurveSegmentLinear() = default;
  
# pragma mark -
# pragma mark Accessors

  typename CurveSegment<T>::Type getType() const override final
  {return CurveSegment<T>::Type::LINEAR;}

};
}
