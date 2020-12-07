// this file comes from https://github.com/Cycling74/max-test/blob/master/source/oscar/test.equals.cpp
//
// 	unit/integration test extension for max
//	tim place
//	cycling '74
//

#include "ext.h"
#include "ext_obex.h"
#ifdef WIN_VERSION
#include <float.h>
#endif
#include <algorithm>

/** like == but can compare floating-point numbers while tolerating the floating-point (im)precision.
 */
typedef struct equals {
  t_object	x_ob;				///< header
  void		*x_outlet;			///< float/list for sampled values
  void		*x_inlet;			///< for setting the operand
  double		x_operand;			///< the number against which to test input
  long		x_tolerance;		///< number of floating-point representations around the specified operand to consider as "equal"
  long		x_single_precision;	///< operate on 32-bit floats rather than 64-bit doubles
} t_ossiaequals;


// prototypes
void*	ossiaequals_new(t_symbol *s, long argc, t_atom *argv);
void 	ossiaequals_free(t_ossiaequals *x);
void	ossiaequals_assist(t_ossiaequals *x, void *b, long m, long a, char *s);
void	ossiaequals_float(t_ossiaequals *x, double f);


// class variables
static t_class		*s_ossiaequals_class = NULL;


/************************************************************************/

extern "C" {
void ossia_equals_setup(void)
{
  t_class *c = class_new("ossia.equals", (method)ossiaequals_new, (method)ossiaequals_free, sizeof(t_ossiaequals), (method)NULL, A_GIMME, 0);

  class_addmethod(c, (method)ossiaequals_float,	"float",	A_FLOAT, 0);
  class_addmethod(c, (method)ossiaequals_assist,	"assist",	A_CANT, 0);

  CLASS_ATTR_LONG(c, "tolerance", 0, t_ossiaequals, x_tolerance);
  CLASS_ATTR_LONG(c, "single_precision", 0, t_ossiaequals, x_single_precision);

  class_register(CLASS_BOX, c);
  s_ossiaequals_class = c;
}
}

/************************************************************************/


void* ossiaequals_new(t_symbol *s, long argc, t_atom *argv)
{
  t_ossiaequals	*x = (t_ossiaequals*)object_alloc(s_ossiaequals_class);
  long			attrstart = attr_args_offset((short)argc, argv);

  if (attrstart)
    x->x_operand = atom_getfloat(argv);

  x->x_outlet = outlet_new(x, NULL);
  x->x_inlet = proxy_new(x, 1, NULL);
  x->x_tolerance = 2;
#ifdef C74_X64
  x->x_single_precision = false;
#else
  x->x_single_precision = true;
#endif

  attr_args_process(x, (short)argc, argv);
  return x;
}


void ossiaequals_free(t_ossiaequals *x)
{
  object_free(x->x_inlet);
}


#pragma mark -
/************************************************************************/

void ossiaequals_assist(t_ossiaequals *x, void *b, long m, long a, char *s)
{
  strcpy(s, "log messages to the test result, or to the max window");
}


// see http://realtimecollisiondetection.net/blog/?p=89 regarding the comparison

static const double k_epsilon64 = DBL_EPSILON;
static const float k_epsilon32 = FLT_EPSILON;

t_bool ossiaequals_equivalent(double a, double b, long tolerance, long single_precision)
{
  if (single_precision) {
    float tol = tolerance * k_epsilon32;
    float maxab = (std::max)(fabsf((float)a), fabsf((float)b));

    if ( fabsf((float)a - (float)b) <= tol * (std::max)(1.0f, maxab) )
      return true;
  }
  else {
    double tol = tolerance * k_epsilon64;
    double maxab = (std::max)(fabs(a), fabs(b));

    if ( fabs(a - b) <= tol * (std::max)(1.0, maxab) )
      return true;
  }
  return false;
}


void ossiaequals_float(t_ossiaequals *x, double f)
{
  if (proxy_getinlet((t_object*)x) == 1)
    x->x_operand = f;
  else
    outlet_int(x->x_outlet, ossiaequals_equivalent(x->x_operand, f, x->x_tolerance, x->x_single_precision));
}

