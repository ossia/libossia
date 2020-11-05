#pragma once
#include <ossia-max/src/node_base.hpp>

namespace ossia
{
namespace max
{

class view : public node_base
{
public:

  using is_view = std::true_type;

  bool do_registration(const std::vector<std::shared_ptr<matcher>>& nodes);
  bool unregister();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::view*);

};

} // max namespace
} // ossia namespace


