#pragma once
#include <ossia-max/src/object_base.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_view structure declaration

class view : public object_base
{
public:
  bool register_node(const std::vector<t_matcher>& nodes);
  bool do_registration(const std::vector<t_matcher>& nodes);
  bool unregister();

  static ossia::safe_set<view*>& quarantine();

  static void register_children(view* x);
  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::view*);

};

} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_view class declaration

