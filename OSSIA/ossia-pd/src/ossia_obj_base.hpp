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

struct t_obj_base;

class t_matcher
{
public:
  t_matcher(ossia::net::node_base* n, t_obj_base* p); // constructor
  ~t_matcher();
  t_matcher(const t_matcher&) = delete;
  t_matcher(t_matcher&& other);
  t_matcher& operator=(const t_matcher&) = delete;
  t_matcher& operator=(t_matcher&& other);

  void set_value(const ossia::value& v);
  auto get_node() const { return node; }

  inline bool operator==(const t_matcher& rhs)
  { return (get_node() == rhs.node); }


  private:
    ossia::net::node_base* node{};
    t_obj_base* parent{};

    ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
        callbackit = ossia::none;

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
  bool x_is_pattern{}; // whether the address is a pattern or not
  bool x_dead = false; // whether this object is being deleted or not

  t_clock* x_clock{};
  t_clock* x_regclock{};   // registration clock
  t_clock* x_unregclock{}; // unregistration clock
  t_canvas* x_last_opened_canvas{};
  std::chrono::milliseconds x_last_click;

  ossia::net::node_base* x_node{};
  std::vector<t_matcher> x_matchers{};

  std::vector<ossia::optional<ossia::callback_container<ossia::value_callback>::iterator> >
      x_callbackits;
  void is_deleted(const ossia::net::node_base& n);

  void setValue(const ossia::value& val);
  static void obj_push(t_obj_base* x, t_symbol*, int argc, t_atom* argv);
  static void obj_bang(t_obj_base* x);
};

bool find_and_display_friend(t_obj_base* x, t_canvas* patcher);
void obj_tick(t_obj_base* x);
void obj_namespace(t_obj_base* x);

}
} // namespace
