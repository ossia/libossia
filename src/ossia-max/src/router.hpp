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
  long m_leadslash{0};

  router();
  ~router();

  void change_pattern(int index, std::string&& pattern);

  static void free(ossia::max_binding::router* x);
  static void
  in_anything(ossia::max_binding::router* x, t_symbol* s, long argc, t_atom* argv);

  static void in_float(ossia::max_binding::router* x, double f);
  static void in_int(ossia::max_binding::router* x, long int f);
  static void in_symbol(ossia::max_binding::router* x, t_symbol* f);
  static void in_list(ossia::max_binding::router* x, t_symbol*, int argc, t_atom* argv);

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
