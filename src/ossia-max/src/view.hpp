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

  void do_registration(const std::vector<std::shared_ptr<matcher>>& nodes);
  void unregister();

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::view*);

};

} // max namespace
} // ossia namespace


