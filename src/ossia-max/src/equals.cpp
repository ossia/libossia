// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// this file comes from
// https://github.com/Cycling74/max-test/blob/master/source/oscar/test.equals.cpp
//
// 	unit/integration test extension for max
//	tim place
//	cycling '74
//
#include "equals.hpp"

#include <ossia-max/src/ossia-max.hpp>

using namespace ossia::max_binding;

extern "C" void ossia_equals_setup()
{
  auto& ossia_library = ossia_max::instance();

  // instantiate the ossia.logger class
  t_class* c = class_new(
      "ossia.equals", (method)ossia_equals_new, (method)equals::free, sizeof(equals),
      (method)NULL, A_GIMME, 0);

  class_addmethod(c, (method)equals::in_float, "float", A_FLOAT, 0);
  class_addmethod(c, (method)equals::assist, "assist", A_CANT, 0);

  CLASS_ATTR_LONG(c, "tolerance", 0, equals, x_tolerance);
  CLASS_ATTR_LONG(c, "single_precision", 0, equals, x_single_precision);

  class_register(CLASS_BOX, c);
  ossia_library.ossia_equals_class = c;
}

extern "C" void* ossia_equals_new(t_symbol* s, long argc, t_atom* argv)
{
  equals* x = (equals*)object_alloc(ossia_max::instance().ossia_equals_class);

  if(x)
  {
    long attrstart = attr_args_offset((short)argc, argv);

    if(attrstart)
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
  }

  return x;
}

void equals::free(equals* x)
{
  object_free(x->x_inlet);
}

void equals::assist(equals* x, void* b, long m, long a, char* s)
{
  strcpy(s, "log messages to the test result, or to the max window");
}

// see http://realtimecollisiondetection.net/blog/?p=89 regarding the comparison

static const double k_epsilon64 = DBL_EPSILON;
static const float k_epsilon32 = FLT_EPSILON;

t_bool equivalent(double a, double b, long tolerance, long single_precision)
{
  if(single_precision)
  {
    float tol = tolerance * k_epsilon32;
    float maxab = (std::max)(fabsf((float)a), fabsf((float)b));

    if(fabsf((float)a - (float)b) <= tol * (std::max)(1.0f, maxab))
      return true;
  }
  else
  {
    double tol = tolerance * k_epsilon64;
    double maxab = (std::max)(fabs(a), fabs(b));

    if(fabs(a - b) <= tol * (std::max)(1.0, maxab))
      return true;
  }
  return false;
}

void equals::in_float(equals* x, double f)
{
  if(proxy_getinlet((t_object*)x) == 1)
    x->x_operand = f;
  else
    outlet_int(
        x->x_outlet, equivalent(x->x_operand, f, x->x_tolerance, x->x_single_precision));
}
