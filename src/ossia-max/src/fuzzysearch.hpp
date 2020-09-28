#pragma once
#include "ext.h"
#include "ext_obex.h"
#undef error
#undef post

#include <ossia/network/base/node.hpp>
#include <ossia/network/base/node_functions.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_fuzzysearch structure declaration

struct fuzzysearch
{
  t_object m_object;
  ossia::net::node_base* m_root{};
  void* m_outlet{};
  std::vector<ossia::net::fuzzysearch_result> m_matches{};

  fuzzysearch(long argc, t_atom* argv);

  static void search(ossia::max::fuzzysearch* x, t_symbol* s, long argc, t_atom* argv);
  static void free(ossia::max::fuzzysearch* x);
  static t_max_err notify(fuzzysearch *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

  static void assist(fuzzysearch* x, void* b, long m, long a, char* s);


};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_fuzzysearch class declaration

extern "C" {
void* ossia_fuzzysearch_new(t_symbol* s, long argc, t_atom* argv);
}
