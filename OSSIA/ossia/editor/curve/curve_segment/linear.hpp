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

namespace ossia
{
template <typename Y>
struct curve_segment_linear
{
  Y operator()(double ratio, Y start, Y end) const
  {
    return start + ratio * (end - start);
  }
};
}
