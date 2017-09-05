#pragma once

#include "ossia_object_base.hpp"

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_view structure declaration

struct t_view : t_object_base
{
  bool register_node(const std::vector<ossia::net::node_base*>& nodes);
  bool do_registration(const std::vector<ossia::net::node_base*>& nodes);
  bool unregister();

  void register_children(t_view* x);
  static void view_bind(t_view* x, t_symbol* address);

  void is_deleted(const ossia::net::node_base& n)
  {
    if (!m_dead)
      unregister();
  }

  static ossia::safe_vector<t_view*>& quarantine();
};

} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_view class declaration

extern "C" {
void* ossia_view_new(t_symbol*, long, t_atom*);
void ossia_view_free(ossia::max::t_view*);
//    void ossia_view_click(t_view *x, t_floatarg xpos, t_floatarg ypos,
//    t_floatarg shift, t_floatarg ctrl, t_floatarg alt);
}
