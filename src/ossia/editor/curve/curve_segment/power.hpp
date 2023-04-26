#pragma once
#include <ossia/editor/curve/curve_segment/easing.hpp>

#include <cmath>
namespace ossia
{
template <typename Y, typename Power>
struct curve_segment_power : easing::power<Power>
{
  Y operator()(double ratio, Y start, Y end) const noexcept
  {
    return ossia::easing::ease{}(
        start, end, ((const easing::power<Power>&)(*this))(ratio));
  }
};
}
