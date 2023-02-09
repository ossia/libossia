#pragma once
#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post

#include "search_filter.hpp"

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>

#include <set>

namespace ossia
{
namespace max_binding
{

#pragma mark -
#pragma mark t_fuzzysearch structure declaration

struct fuzzysearch : search_filter
{
  t_object m_object;
  t_symbol* m_scope{};

  void* m_outlet{};
  long m_case_sensitive{};

  std::set<ossia::net::node_base*> m_roots;
  std::vector<std::string> patterns;
  std::vector<ossia::net::fuzzysearch_result> matches;

  fuzzysearch();
  ~fuzzysearch();

  static void
  search(ossia::max_binding::fuzzysearch* x, t_symbol* s, long argc, t_atom* argv);
  static void free(ossia::max_binding::fuzzysearch* x);
  static t_max_err
  notify(fuzzysearch* x, t_symbol* s, t_symbol* msg, void* sender, void* data);

  static void assist(fuzzysearch* x, void* b, long m, long a, char* s);
};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_fuzzysearch class declaration

extern "C" {
void* ossia_fuzzysearch_new(t_symbol* s, long argc, t_atom* argv);
}
