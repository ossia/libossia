#pragma once

#include "Editor/CurveSegment.h"
#include <Editor/CurveSegment/MathUtil.h>
#include <cmath>
namespace OSSIA
{

namespace OSSIA
{
template <typename Y>
struct CurveSegmentSin
{
  CurveSegment<Y> operator()(double freq, double phase, double ampl) const
  {
      return [=] (double ratio, Y start, Y end) {
          return start + ampl * std::sin(phase + two_pi * ratio * freq) * (end - start);
      };
  }
};
}
