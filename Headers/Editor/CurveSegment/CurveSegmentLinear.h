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

template <typename Y>
class CurveSegmentLinear : public CurveSegment<Y>
{

public:

#if 0
# pragma mark -
# pragma mark Life cycle
#endif

  /*! factory
   \param std::shared_ptr<CurveAbstract> parent
   \return std::shared_ptr<CurveSegmentLinear<Y>> */
  static std::shared_ptr<CurveSegmentLinear<Y>> create(std::shared_ptr<CurveAbstract> = nullptr);

  /*! clone */
  virtual std::shared_ptr<CurveSegmentLinear<Y>> clone() const = 0;

  /*! destructor */
  virtual ~CurveSegmentLinear();

#if 0
# pragma mark -
# pragma mark Accessors
#endif

  typename CurveSegment<Y>::Type getType() const override final
  {return CurveSegment<Y>::Type::LINEAR;}

};
}


#if !defined(APIJamoma_EXPORTS)
extern template class OSSIA_EXPORT OSSIA::CurveSegmentLinear<bool>;
extern template class OSSIA_EXPORT OSSIA::CurveSegmentLinear<int>;
extern template class OSSIA_EXPORT OSSIA::CurveSegmentLinear<float>;
#endif
