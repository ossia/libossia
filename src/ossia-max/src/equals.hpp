#pragma once

#include "ext.h"
#include "ext_obex.h"
#ifdef WIN_VERSION
#include <float.h>
#endif
#include <algorithm>

namespace ossia
{
namespace max
{
/** like == but can compare floating-point numbers while tolerating the floating-point (im)precision.
 */
typedef struct equals {
  t_object	x_ob;            	///< header
  void		*x_outlet;        	///< float/list for sampled values
  void		*x_inlet;         	///< for setting the operand
  double		x_operand;      	///< the number against which to test input
  long		x_tolerance;    		///< number of floating-point representations around the specified operand to consider as "equal"
  long		x_single_precision;	///< operate on 32-bit floats rather than 64-bit doubles

  // prototypes
  static void 	free(equals *x);
  static void   assist(equals *x, void *b, long m, long a, char *s);
  static void   in_float(equals *x, double f);
};

extern "C"
{
  void*	ossia_equals_new(t_symbol *s, long argc, t_atom *argv);
}

} // namespace max
} // namespace ossia
