#pragma once
#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post

#include <vector>
#include <string>
#include <regex>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark router structure declaration

struct router
{
  t_object m_object;

  router(long argc, t_atom* argv);
  void add_pattern(std::string pattern);

  static void free(ossia::max::router* x);
  static void in_anything(ossia::max::router* x, t_symbol* s, long argc, t_atom* argv);

  static void assist(router* x, void* b, long m, long a, char* s);

  std::vector<std::regex> m_patterns{};
  std::vector<void*> m_outlets{};
  void* m_inlet{};

};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_router class declaration

extern "C" {
  void* ossia_router_new(t_symbol* s, long argc, t_atom* argv);
}
