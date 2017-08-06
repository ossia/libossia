#pragma once
#include <ossia/detail/safe_vec.hpp>
#include <ossia/ossia.hpp>

extern "C" {
#include <cicm_wrapper.h>
}

namespace ossia
{
namespace pd
{

enum class Type {
  root = 0,
  param,
  remote,
  model,
  view,
  device,
  client
};

enum class AddrScope
{
  relative = 0,
  absolute,
  global
};

struct t_obj_base
{
  t_eobj x_obj;
  Type x_otype{};
  t_symbol* x_name{};
  t_outlet* x_setout{};
  t_outlet* x_dataout{};
  t_outlet* x_dumpout{};
  AddrScope x_addr_scope{};
  bool x_dead = false; // wether this object is being deleted or not;

  t_clock* x_clock{};
  t_clock* x_regclock{};   // registration clock
  t_clock* x_unregclock{}; // unregistration clock
  t_canvas* x_last_opened_canvas{};
  std::chrono::milliseconds x_last_click;

  ossia::net::node_base* x_node{};
  void setValue(const ossia::value& val);
  static void obj_push(t_obj_base* x, t_symbol*, int argc, t_atom* argv);
  static void obj_bang(t_obj_base* x);
};

bool find_and_display_friend(t_obj_base* x, t_canvas* patcher);
void obj_tick(t_obj_base* x);
void obj_namespace(t_obj_base* x);

}
} // namespace
