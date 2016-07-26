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

#include "Editor/CurveSegment.hpp"

namespace OSSIA
{
template <typename Y>
struct CurveSegmentLinear
{
  CurveSegment<Y> operator()() const
  {
      return [] (double ratio, Y start, Y end) {
          return start + ratio * (end - start);
      };
  }
};
}
