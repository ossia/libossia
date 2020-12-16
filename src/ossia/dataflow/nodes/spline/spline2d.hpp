#pragma once
#include <vector>

namespace ossia
{
struct spline_point
{
  double x;
  double y;

  friend bool operator==(const spline_point& lhs, const spline_point& rhs)
  {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }
  friend bool operator!=(const spline_point& lhs, const spline_point& rhs)
  {
    return lhs.x != rhs.x || lhs.y != rhs.y;
  }
};

struct spline_data
{
  std::vector<spline_point> points;

  friend bool operator==(const spline_data& lhs, const spline_data& rhs)
  {
    return lhs.points == rhs.points;
  }
  friend bool operator!=(const spline_data& lhs, const spline_data& rhs)
  {
    return lhs.points != rhs.points;
  }
};
}
