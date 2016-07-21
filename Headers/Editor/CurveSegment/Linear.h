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
struct OSSIA_EXPORT CurveSegmentLinear final : public CurveSegment<Y>
{
  Y valueAt(double ratio, Y start, Y end) const override
  {
    return start + ratio * (end - start);
  }
};
}
