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

#include "Editor/CurveSegment.hpp"
#include <cmath>

namespace OSSIA
{
template <typename Y>
struct CurveSegmentPower
{
  CurveSegment<Y> operator()(double power) const
  {
      return [=] (double ratio, Y start, Y end) {
          return start + std::pow(ratio, power) * (end - start);
      };
  }
};
}
