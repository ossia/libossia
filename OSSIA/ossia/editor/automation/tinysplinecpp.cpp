#include "tinysplinecpp.h"

/********************************************************
*                                                       *
* Utils                                                 *
*                                                       *
********************************************************/
tinyspline::BSpline tinyspline::Utils::interpolateCubic(
    const std::vector<tinyspline::real>* points, const size_t dim)
{
  if (dim == 0)
    throw std::runtime_error(ts_enum_str(TS_DIM_ZERO));
  if (points->size() % dim != 0)
    throw std::runtime_error("#points % dim == 0 failed");
  tinyspline::BSpline bspline;
  const tsError err = ts_bspline_interpolate_cubic(
      points->data(), points->size() / dim, dim, bspline.data());
  if (err < 0)
    throw std::runtime_error(ts_enum_str(err));
  return bspline;
}

bool tinyspline::Utils::fequals(
    const tinyspline::real x, const tinyspline::real y)
{
  return ts_fequals(x, y) == 1;
}

std::string tinyspline::Utils::enum_str(const tsError err)
{
  return std::string(ts_enum_str(err));
}

tsError tinyspline::Utils::str_enum(const std::string str)
{
  return ts_str_enum(str.c_str());
}
