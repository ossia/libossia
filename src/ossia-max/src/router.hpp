#pragma once
#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post

#include <regex>
#include <string>
#include <vector>

namespace ossia
{
namespace max_binding
{

#pragma mark -
#pragma mark router structure declaration

struct router
{
  t_object m_object;
  long m_truncate{1};

  router(long argc, t_atom* argv);
  void change_pattern(int index, std::string pattern);

  static void free(ossia::max_binding::router* x);
  static void
  in_anything(ossia::max_binding::router* x, t_symbol* s, long argc, t_atom* argv);

  static void assist(router* x, void* b, long m, long a, char* s);

  struct pattern {
      std::string pattern;
      std::regex regex;
      bool simple{};
  };

  std::vector<pattern> m_patterns{};
  std::vector<void*> m_outlets{};
  std::vector<void*> m_inlets{};
};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_router class declaration

extern "C" {
void* ossia_router_new(t_symbol* s, long argc, t_atom* argv);
}
