/** @file TTInterpolate.h
 *
 * @ingroup foundationLibrary
 *
 * @brief Interpolation Utilities

 * @details Defines several functions for <a href="http://en.wikipedia.org/wiki/Interpolation">interpolating</a> between discrete data points such as those found in an array or matrix. These methods are commonly used in digital audio whenever we alter the rate at which a signal is read.
 * These functions require known discrete values to be passed by reference along with a double between 0 and 1 representing the fractional location desired. They return the interpolated value.
 * 
 * @authors Timothy Place
 *
 * @copyright Copyright © 2012, Timothy Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_INTERPOLATE_H__
#define __TT_INTERPOLATE_H__

#include "TTBase.h"

/** Isolate the fractional part from a double.
	Essentially wraps the <a href="http://www.cplusplus.com/reference/clibrary/cmath/modf/">modf()</a> function, but protects Jamoma in case it ever prooves uneven in implementation. NW: FAILS TO BUILD but Xcode can't find error.
	@param aa		Double whose fractional part you would like.
	@return			The fractional portion of aa.

double TTSplitFractional(double& aa)
{
	double discard;
	return modf(aa,&discard);
}
*/


/** Linear interpolation.
 @param x0		Sample value at prior integer index
 @param x1		Sample value at next integer index
 @param delta 	Linear interpolation between x0 (delta=0) and x1 (delta=1)
 @return			The interpolated value.
 
 @seealso		TTInterpolateCosine
 @seealso		TTInterpolateCubic
 @seealso		TTInterpolateSpline
 @seealso		TTInterpolateHermite
*/
template<class T>
T TTInterpolateLinear(const T& x0, const T& x1, const double& delta)
{
	return x0 + delta * (x1-x0);
}


/** Cosine interpolation
	@param x	Sample value at prior integer index
	@param y	Sample value at next integer index
	@param a 	Fractional location between x (0) and y (1)
	@return 	The interpolated value.
	
	@seealso	TTInterpolateLinear	
	@seealso	TTInterpolateHermite
	@seealso	TTInterpolateCubic	
	@seealso	TTInterpolateSpline	
 */
template<class T>
T TTInterpolateCosine(const T& x, const T& y, const double& a)
{
	T a2 = 0.5 * (1.0 - cos(a * kTTPi));	
	return x + a2 * (y-x);
}


/** Cubic interpolation
 
 @details This interpolation algorithms calculate the coefficients a, b, c, d
 of the 3rd order polynomial
 
 f(delta)	= a*aDelta^3 + b*aDelta^2 + c*aDelta + d
			= ( (a*aDelta + b )*aDelta + c)*aDelta + d)
 
so that the function fulfill the following four conditions:
 
 -# f(0)  = x1 @n
 -# f(1)  = x2 @n
 -# f'(0) = (x2-x0)/2 @n
 -# f'(1) = (x3-x1)/2
 
 The two last conditions use a symetric estimate of the difference at the end points
 of the region to interpolate over: 0 ≤ aDelta ≤ 1
 
 These asumptions ensure that the resulting interpolated function, when moving over several 
 subsequent sections, is:
 
 -# Continuous (no sudden jump)
 -# Has a continuous derivative (no break pints with hard edges)
 
 However, the 2nd order derivate will generally be discontinuous on the points connecting two sections.
 
 
 
 @param x0		Sample value at integer index prior to x0
 @param x1		Sample value at prior integer index
 @param x2		Sample value at next integer index
 @param x3		Sample value at integer index after x2
 @param aDelta	Fractional location where we want to do the interpolation. @n
				aDelta = 0 => interpolatedeValue = x1 @n
				aDelta = 1 => interpolatedeValue = x2
 @return		The interpolated value.
	
 @seealso		TTInterpolateLinear
 @seealso		TTInterpolateCosine
 @seealso		TTInterpolateHermite
 @seealso		TTInterpolateSpline
*/
template<class T>
T TTInterpolateCubic(const T& x0, const T& x1, const T& x2, const T& x3, const double& aDelta)
{
	T a = (-x0 + 3.*x1 - 3*x2 + x3)*0.5;
	T b = x0 - 2.5*x1 + 2.*x2 - 0.5*x3;
	T c = (x2-x0)*0.5;
	
	//T a = x3 - x2 - x0 + x1;
	//T b = x0 - x1 - f0;
	//T c = x2 - x0;
	// T d = x1;
	return ( (a*aDelta + b)*aDelta + c)*aDelta + x1;
}


/** Spline interpolation based on the Breeuwsma catmull-rom spline 
 @param w	Sample value at integer index prior to x
 @param x	Sample value at prior integer index
 @param y	Sample value at next integer index
 @param z	Sample value at integer index after y
 @param a	Fractional location between x (0) and y (1)
 @return		The interpolated value.
	
 @seealso	TTInterpolateLinear
 @seealso	TTInterpolateCosine
 @seealso	TTInterpolateCubic
 @seealso	TTInterpolateHermite
*/
template<class T>
T TTInterpolateSpline(const T& w, const T& x, const T& y, const T& z, const double& a)
{
	T a2 = a*a;
	T f0 = -0.5*w + 1.5*x - 1.5*y + 0.5*z;
	T f1 = w - 2.5*x + 2.0*y - 0.5*z;
	T f2 = -0.5*w + 0.5*y;
	return f0*a*a2 + f1*a2 + f2*a + x;
}


/** Hermite interpolation 
 @param w	Sample value at integer index prior to x
 @param x	Sample value at prior integer index
 @param y	Sample value at next integer index
 @param z	Sample value at integer index after y
 @param a	Fractional location between x (0) and y (1)
 @return		The interpolated value.

 @seealso	TTInterpolateLinear
 @seealso	TTInterpolateCosine
 @seealso	TTInterpolateCubic
 @seealso	TTInterpolateSpline
*/
template<class T>
T TTInterpolateHermite(const T& w, const T& x, const T& y, const T& z, const double& a, const double& bias, const double& tension)
{
	T aa = a*a;
	T aaa = a*aa;
	T bp = 1+bias;
	T bm = 1-bias;
	T mt = (1-tension)*0.5;
	T m0 = ((x-w)*bp + (y-x)*bm) * mt;
	T m1 = ((y-x)*bp + (z-y)*bm) * mt;
	T a0 = 2*aaa - 3*aa + 1;
   	T a1 = aaa - 2*aa + a;
   	T a2 = aaa - aa;
   	T a3 = -2*aaa + 3*aa;
   	return a0*x + a1*m0 + a2*m1 + a3*y;
}


#endif // __TT_INTERPOLATE_H__

