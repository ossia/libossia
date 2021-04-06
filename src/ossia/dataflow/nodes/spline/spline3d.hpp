#pragma once
#include <vector>

namespace ossia
{
struct spline3d_point
{
  double x;
  double y;
  double z;

  friend bool operator==(const spline3d_point& lhs, const spline3d_point& rhs)
  {
    return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
  }
  friend bool operator!=(const spline3d_point& lhs, const spline3d_point& rhs)
  {
    return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
  }
};

struct spline3d_data
{
  std::vector<spline3d_point> points;

  friend bool operator==(const spline3d_data& lhs, const spline3d_data& rhs)
  {
    return lhs.points == rhs.points;
  }
  friend bool operator!=(const spline3d_data& lhs, const spline3d_data& rhs)
  {
    return lhs.points != rhs.points;
  }
};
}
