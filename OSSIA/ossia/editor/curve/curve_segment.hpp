#pragma once
#include <functional>

namespace OSSIA
{

/*! get value at an abscissa
 \param double ratio between 0. and 1.
 \param Y segment start value
 \param Y segment end value
 \return Y value */
template <typename Y>
using CurveSegment = std::function<Y(double, Y, Y)>;

}
