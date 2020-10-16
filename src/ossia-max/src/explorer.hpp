#pragma once
#include <ossia-max/src/object_base.hpp>
#include "search_filter.hpp"

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

  void stop_monitoring();

  bool register_node(const std::vector<std::shared_ptr<t_matcher>>& node);
  bool unregister();
  void parse_args(t_symbol* s, long argc, t_atom* argv);

  // device callbacks
  void on_node_created_callback(const ossia::net::node_base& node);
  void on_node_removing_callback(const ossia::net::node_base& node);
  void on_node_renamed_callback(const ossia::net::node_base& node, const std::string& name);
  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  void on_parameter_removing_callback(const ossia::net::parameter_base& addr);
  void on_device_deleted(const ossia::net::node_base&);
  void handle_modification(const ossia::net::node_base& node, t_symbol* type, t_symbol* action);

  ossia::safe_set<explorer*>& quarantine();

  static void execute_method(ossia::max::explorer* x, t_symbol* s, long argc, t_atom* argv);
  static void free(ossia::max::explorer* x);
  static t_max_err notify(explorer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
  static void assist(explorer* x, void* b, long m, long a, char* s);

  static t_symbol* s_explore;
  static t_symbol* s_search;
  static t_symbol* s_monitor;
  static t_symbol* s_size;
  static t_symbol* s_namespace;
  static t_symbol* s_parameter;
  static t_symbol* s_node;
  static t_symbol* s_created;
  static t_symbol* s_removing;
  static t_symbol* s_renamed;
  static t_symbol* s_open;
};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_explorer class declaration

extern "C" {
void* ossia_explorer_new(t_symbol* s, long argc, t_atom* argv);
}
