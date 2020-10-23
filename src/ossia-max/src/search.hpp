#pragma once
#include <ossia-max/src/object_base.hpp>
#include "search_filter.hpp"
#include <set>

namespace ossia
{
namespace max
{

#define MAX_NUM_ITEMS 256

#pragma mark -
#pragma mark t_search structure declaration

struct search : object_base, search_filter
{
  using is_search = std::true_type;

  t_symbol* m_sort{};
  t_symbol* m_format{};
  t_symbol* m_method{};

  std::set<ossia::net::device_base*> m_devices;

  // ctor / dtor
  search(long argc, t_atom* argv);
  ~search();

  bool register_node(std::vector<std::shared_ptr<t_matcher>>& node);
  bool unregister();
  void parse_args(t_symbol* s, long argc, t_atom* argv);

  static void execute_method(ossia::max::search* x, t_symbol* s, long argc, t_atom* argv);
  static void free(ossia::max::search* x);
  static t_max_err notify(search *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
  static void assist(search* x, void* b, long m, long a, char* s);

  static t_symbol* s_search;
  static t_symbol* s_size;
  static t_symbol* s_result;
  static t_symbol* s_open;
};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_search class declaration

extern "C" {
void* ossia_search_new(t_symbol* s, long argc, t_atom* argv);
}
