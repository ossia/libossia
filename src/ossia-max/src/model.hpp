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

  long m_autoname;

  void do_registration();
  void save_children_state();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::model*);
  static void assist(ossia::max::model*, void*, long, long, char*);
};

} // max namespace
} // ossia namespace

