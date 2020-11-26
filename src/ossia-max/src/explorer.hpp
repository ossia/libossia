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
#pragma mark t_explorer structure declaration

struct explorer : object_base, search_filter
{
  using is_explorer = std::true_type;

  // argument variables
  long m_highlight{};
  long m_depth{0}; // 0 means no depth filtering

  t_symbol* m_sort{};
  t_symbol* m_format{};
  t_symbol* m_method{};

  std::set<ossia::net::device_base*> m_devices;

  // ctor / dtor
  explorer(long argc, t_atom* argv);
  ~explorer();

  bool register_node(std::vector<std::shared_ptr<matcher>>& node);
  bool unregister();

  static void explore_mess_cb(ossia::max::explorer* x, t_symbol* s, long argc, t_atom* argv);
  static void free(ossia::max::explorer* x);
  static t_max_err notify(explorer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
  static void assist(explorer* x, void* b, long m, long a, char* s);

  static t_symbol* s_explore;
  static t_symbol* s_size;
  static t_symbol* s_namespace;
};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_explorer class declaration

extern "C" {
void* ossia_explorer_new(t_symbol* s, long argc, t_atom* argv);
}
