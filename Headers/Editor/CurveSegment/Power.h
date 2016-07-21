/*!
 * \file CurveSegmentPower.h
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
#include <cmath>
namespace OSSIA
{
template <typename Y>
struct OSSIA_EXPORT CurveSegmentPower final : public CurveSegment<Y>
{
  Y valueAt(double ratio, Y start, Y end) const override
  {
    return start + std::pow(ratio, power) * (end - start);
  }

  double power{1.};
};
}
