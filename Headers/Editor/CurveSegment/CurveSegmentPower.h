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
class OSSIA_EXPORT CurveSegmentPower final : public CurveSegment<Y>
{
public:

#if 0
# pragma mark -
# pragma mark Accessors
#endif
  Y valueAt(double ratio, Y start, Y end) const override
  {
    return start + std::pow(ratio, mPower) * (end - start);
  }

  /*! get power value
  \return double */
  double getPower() const { return mPower; }

  /*! set power value
  \param double
  \return CurveSegmentPower */
  CurveSegmentPower<Y> & setPower(double v)
  {
    mPower = v;
    return *this;
  }

private:
  double mPower{1};
};
}
