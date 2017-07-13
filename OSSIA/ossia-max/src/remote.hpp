#pragma once
#include "ossia_object_base.hpp"
#include <boost/optional.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_remote structure declaration

struct t_remote : t_object_base
{
  boost::optional<ossia::callback_container<ossia::value_callback>::iterator>
      m_callbackit;

  bool register_node(ossia::net::node_base* node);
  bool do_registration(ossia::net::node_base* node);
  bool unregister();

  void is_deleted(const ossia::net::node_base& n)
  {
    if (!m_dead)
      unregister();
  }

  static ossia::safe_vector<t_remote*>& quarantine()
  {
    static ossia::safe_vector<t_remote*> quarantine;
    return quarantine;
  }
};

} // max namespace
} // ossia namespace

#pragma mark -
#pragma mark ossia_remote class declaration

extern "C" {
void* ossia_remote_new(t_symbol*, long, t_atom*);
void ossia_remote_free(ossia::max::t_remote*);

//    void ossia_remote_click(t_remote *x, t_floatarg xpos, t_floatarg ypos,
//    t_floatarg shift, t_floatarg ctrl, t_floatarg alt);
void ossia_remote_assist(ossia::max::t_remote*, void*, long, long, char*);

void ossia_remote_in_float(ossia::max::t_remote*, double f);
void ossia_remote_in_int(ossia::max::t_remote*, long int f);
void ossia_remote_in_bang(ossia::max::t_remote*);
void ossia_remote_in_symbol(ossia::max::t_remote*, t_symbol*);
void ossia_remote_in_char(ossia::max::t_remote*, char);
void ossia_remote_in_anything(ossia::max::t_remote*, t_symbol*, long, t_atom*);
}
