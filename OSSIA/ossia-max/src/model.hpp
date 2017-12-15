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
  bool register_node(const std::vector<t_matcher>& nodes);
  bool do_registration(const std::vector<t_matcher>& nodes);
  bool unregister();
  void register_children();

  static ossia::safe_set<model*>& quarantine();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::model*);
  static void assist(ossia::max::model*, void*, long, long, char*);
  static t_max_err notify(model*x, t_symbol*s, t_symbol* msg, void* sender, void* data);
};

} // max namespace
} // ossia namespace

