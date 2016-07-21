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
class OSSIA_EXPORT CurveSegmentEmpty final : public CurveSegment<Y>
{
  Y valueAt(double ratio, Y start, Y end) const
  {
    return start;
  }
};
}
