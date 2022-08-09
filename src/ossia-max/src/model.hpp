#pragma once
#include <ossia-max/src/node_base.hpp>

namespace ossia
{
namespace max_binding
{

class model : public node_base
{
public:
  using is_model = std::true_type;

  void do_registration();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max_binding::model*);
  static void assist(ossia::max_binding::model*, void*, long, long, char*);
};

} // max namespace
} // ossia namespace
