#pragma once
#include <ossia-max/src/object_base.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_explorer structure declaration

struct explorer : object_base
{
  using is_explorer = std::true_type;

  t_object m_object;
  t_symbol* m_method{};
  t_symbol* m_node_name{};
  std::set<ossia::net::device_base*> m_devices;

  // argument variables
  long m_highlight{};
  t_symbol* m_type{};
  t_symbol* m_sort{};
  t_symbol* m_format{};

  // ctor
  explorer(long argc, t_atom* argv);

  void stop_monitoring();

  bool register_node(const std::vector<std::shared_ptr<t_matcher>>& node);
  // bool do_registration(const std::vector<std::shared_ptr<t_matcher>>& node);
  bool unregister();
  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  void on_device_deleted(const ossia::net::node_base&);

  static void execute_method(ossia::max::explorer* x, t_symbol* s, long argc, t_atom* argv);
  static void free(ossia::max::explorer* x);
  static t_max_err notify(explorer *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
  static void assist(explorer* x, void* b, long m, long a, char* s);

  static t_symbol* s_explore;
  static t_symbol* s_search;
  static t_symbol* s_monitor;
};
} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_explorer class declaration

extern "C" {
void* ossia_explorer_new(t_symbol* s, long argc, t_atom* argv);
}
