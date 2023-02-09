#pragma once
#include "search_filter.hpp"

#include <ossia-max/src/object_base.hpp>

#include <set>

namespace ossia
{
namespace max_binding
{

#define MAX_NUM_ITEMS 256

#pragma mark -
#pragma mark t_monitor structure declaration

struct monitor
    : object_base
    , search_filter
{
  using is_monitor = std::true_type;

  // argument variables
  long m_highlight{};
  long m_depth{0}; // 0 means no depth filtering

  t_symbol* m_sort{};
  t_symbol* m_format{};
  t_symbol* m_method{};

  std::set<ossia::net::device_base*> m_devices;

  // ctor / dtor
  monitor();
  ~monitor();

  void stop_monitoring();

  bool unregister();
  void parse_args(t_symbol* s, long argc, t_atom* argv);

  // device callbacks
  void on_node_created_callback(const ossia::net::node_base& node);
  void on_node_removing_callback(const ossia::net::node_base& node);
  void
  on_node_renamed_callback(const ossia::net::node_base& node, const std::string& name);
  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  void on_parameter_removing_callback(const ossia::net::parameter_base& addr);
  void on_device_deleted(const ossia::net::node_base&);
  void handle_modification(
      const ossia::net::node_base& node, t_symbol* type, t_symbol* action);

  static void
  execute_method(ossia::max_binding::monitor* x, t_symbol* s, long argc, t_atom* argv);
  static void free(ossia::max_binding::monitor* x);
  static t_max_err
  notify(monitor* x, t_symbol* s, t_symbol* msg, void* sender, void* data);
  static void assist(monitor* x, void* b, long m, long a, char* s);

  static t_symbol* s_monitor;
  static t_symbol* s_parameter;
  static t_symbol* s_node;
  static t_symbol* s_created;
  static t_symbol* s_removing;
  static t_symbol* s_renamed;
};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_monitor class declaration

extern "C" {
void* ossia_monitor_new(t_symbol* s, long argc, t_atom* argv);
}
