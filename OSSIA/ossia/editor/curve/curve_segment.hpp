#pragma once
#include <functional>

namespace ossia
{

/*! get value at an abscissa
 \param double ratio between 0. and 1.
 \param Y segment start value
 \param Y segment end value
 \return Y value */
template <typename Y>
using curve_segment = std::function<Y(double, Y, Y)>;

}
