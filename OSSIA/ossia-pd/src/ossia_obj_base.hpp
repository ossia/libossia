#pragma once
#include <ossia/detail/safe_vec.hpp>
#include <ossia/ossia.hpp>

extern "C" {
#include <cicm_wrapper.h>
}

#define OSSIA_PD_MAX_ATTR_SIZE 256

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

enum class address_scope
{
  relative = 0,
  absolute,
  global
};

struct t_obj_base;

class t_select_clock
{
public:
  t_select_clock(t_canvas* canvas, t_obj_base* obj);
  ~t_select_clock();

  static void deselect(t_select_clock* x);
private:
  t_clock* m_clock{};
  t_obj_base* m_obj{};
  t_canvas* m_canvas{};

};

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
  auto get_parent() const { return parent; }
  void set_parent_addr();

  inline bool operator==(const t_matcher& rhs)
  { return (get_node() == rhs.node); }


private:
  ossia::net::node_base* node{};
  t_obj_base* parent{};

  ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
    callbackit = ossia::none;

  t_atom m_addr;

};

struct t_obj_base
{
  t_eobj m_obj;
  Type m_otype{};
  t_symbol* m_name{};
  t_outlet* m_setout{};
  t_outlet* m_dataout{};
  t_outlet* m_dumpout{};
  address_scope m_addr_scope{};
  bool m_is_pattern{}; // whether the address is a pattern or not
  bool m_dead{}; // whether this object is being deleted or not
  bool m_is_deleted{}; // true during the is_deleted callback method
  bool m_mute{};

  t_clock* m_clock{};
  t_clock* m_regclock{};   // registration clock
  t_clock* m_unregclock{}; // unregistration clock
  std::chrono::milliseconds m_last_click{};

  ossia::optional<ossia::unit_t> m_ounit;

  ossia::net::generic_device* m_device{};
  ossia::net::node_base* m_node{};
  ossia::net::node_base* m_parent_node{};
  std::vector<t_matcher> m_matchers{};

  std::vector<ossia::optional<ossia::callback_container<ossia::value_callback>::iterator> >
      m_callbackits;
  void is_deleted(const ossia::net::node_base& n);

  // TODO why some methods are inside t_obj_base class and other are outside ?
  static void obj_push(t_obj_base* x, t_symbol*, int argc, t_atom* argv);
  static void obj_bang(t_obj_base* x);
};

/**
 * @brief find_and_display_friend go through all registered parameters to find the ones that matches current remote
 * @param x the remote
 * @return false if nothing have been found
 */
bool find_and_display_friend(t_obj_base* x);

/**
 * @brief obj_namespace send the namespace through dump outlet
 * @note only relevant for client, device, model and view objects.
 * @param x
 */
void obj_namespace(t_obj_base* x);

/**
 * @brief obj_set
 * @param x
 * @param s
 * @param argc
 * @param argv
 */
void obj_set(t_obj_base* x, t_symbol* s, int argc, t_atom* argv);

/**
 * @brief obj_get_address return global address through dump outlet
 * @param x
 */
void obj_get_address(t_obj_base *x);

void obj_preset(t_obj_base *x, t_symbol* s, int argc, t_atom* argv);
}
} // namespace
