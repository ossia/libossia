#pragma once

#include "Editor/CurveSegment.h"
#include <Editor/CurveSegment/MathUtil.h>
#include <cmath>
namespace OSSIA
{
template <typename Y>
class OSSIA_EXPORT CurveSegmentPower final : public CurveSegment<Y>
{
public:
  Y valueAt(double ratio, Y start, Y end) const override
  {
    return start + 0.5 * mAmplitude * std::sin(mPhase + two_pi * ratio * mFreq) * (end - start);
  }

  double frequency{3.}; // In hertz
  double phase{0.};
  double amplitude{1.};
};
}
