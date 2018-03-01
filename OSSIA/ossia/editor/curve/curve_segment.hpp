#pragma once
#include <smallfun.hpp>

/**
 * \file curve_segment.hpp
 */

namespace ossia
{
template <typename Y>
/**
 * \typedef curve_segment
 *
 * A curve segment is a function that maps a point between a start and an end.
 *
 * Ex. :
 * \code
 * double my_fun(double x, double start, double end)
 * {
 *   return start + x * (end - start);
 * }
 * \endcode
 *
 * is the basic ease function that interpolates from start to end
 * when x is between [0; 1].
 *
 * In our case x can sometimes be greater than 1.
 */
#if defined(_MSC_VER)
using curve_segment = smallfun::function<Y(double, Y, Y), 24+24>;
#else
using curve_segment = smallfun::function<Y(double, Y, Y), 24>;
#endif
}
