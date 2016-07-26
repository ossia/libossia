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

namespace OSSIA
{
template <typename Y>
struct CurveSegmentLinear
{
  Y operator()(double ratio, Y start, Y end) const
  {
    return start + ratio * (end - start);
  }
};
}
