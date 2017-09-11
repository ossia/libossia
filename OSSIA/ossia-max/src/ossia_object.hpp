#pragma once
#include <ossia-max/src/ossia-max.hpp>

namespace ossia
{
namespace max
{

class ossia_object : public device
{
public:
  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::ossia_object*);
};

} // max namespace
} // ossia namespace
