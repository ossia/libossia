#pragma once
#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post

#include <vector>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_address_router structure declaration

struct address_router
{
  t_object m_object;

  address_router(long argc, t_atom* argv);

  static void free(ossia::max::address_router* x);
  static void in_anything(ossia::max::address_router* x, t_symbol* s, long argc, t_atom* argv);

  static void assist(address_router* x, void* b, long m, long a, char* s);

  std::vector<std::string> m_patterns{};
  std::vector<void*> m_outlets{};

};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_address_router class declaration

extern "C" {
  void* ossia_address_router_new(t_symbol* s, long argc, t_atom* argv);
}
