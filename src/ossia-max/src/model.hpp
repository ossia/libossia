#pragma once
#include <ossia-max/src/node_base.hpp>

namespace ossia
{
namespace max
{

class model : public node_base
{
public:

  using is_model = std::true_type;

  bool register_node(const std::vector<std::shared_ptr<matcher>>& nodes);
  bool do_registration(const std::vector<std::shared_ptr<matcher>>& nodes);
  bool unregister();
  void register_children();
  void save_children_state();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::model*);
  static void assist(ossia::max::model*, void*, long, long, char*);
};

} // max namespace
} // ossia namespace

