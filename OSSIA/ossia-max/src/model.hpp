#pragma once
#include <ossia-max/src/node_base.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_model structure declaration

class model : public node_base
{
public:
  model();

  bool register_node(const std::vector<ossia::net::node_base*>& nodes);
  bool do_registration(const std::vector<ossia::net::node_base*>& nodes);
  bool unregister();
  void register_children();

  static ossia::safe_vector<model*>& quarantine();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::model*);
  static void assist(ossia::max::model*, void*, long, long, char*);
  static t_max_err notify(model*x, t_symbol*s, t_symbol* msg, void* sender, void* data);
};

} // max namespace
} // ossia namespace

