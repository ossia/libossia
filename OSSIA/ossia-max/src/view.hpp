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
  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  void register_children(t_view* x);

  void is_deleted(const ossia::net::node_base& n)
  {
    if (!m_dead)
      unregister();
  }

  static ossia::safe_vector<t_view*>& quarantine()
  {
    static ossia::safe_vector<t_view*> quarantine;
    return quarantine;
  }
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
