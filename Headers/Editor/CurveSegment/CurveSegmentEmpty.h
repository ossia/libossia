/*!
 * \file CurveSegmentEmpty.h
 *
 * \defgroup Editor
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once

#include "Editor/CurveSegment.h"

namespace OSSIA
{

template <typename Y>
class CurveSegmentEmpty : public CurveSegment<Y>
{

public:

# pragma mark -
# pragma mark Life cycle
  
  /*! factory
   \param std::shared_ptr<CurveAbstract> parent
   \return std::shared_ptr<CurveSegmentEmpty<Y>> */
  static std::shared_ptr<CurveSegmentEmpty<Y>> create(std::shared_ptr<CurveAbstract> = nullptr);

  /*! clone */
  virtual std::shared_ptr<CurveSegmentEmpty<Y>> clone() const = 0;
  
  /*! destructor */
  virtual ~CurveSegmentEmpty();
  
# pragma mark -
# pragma mark Accessors

  typename CurveSegment<Y>::Type getType() const override final
  {return CurveSegment<Y>::Type::EMPTY;}

};
}
