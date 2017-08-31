#pragma once
#include <ossia/detail/safe_vec.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/network/base/value_callback.hpp>
#include <ossia/detail/callback_container.hpp>
#include <ossia/editor/dataspace/dataspace.hpp>
extern "C" {
#include <cicm_wrapper.h>
}
#include <readerwriterqueue.h>

#define OSSIA_PD_MAX_ATTR_SIZE 256

namespace ossia
{
namespace net {
class node_base;
class parameter_base;
class device_base;
class generic_device;
}
namespace pd
{

enum class object_class {
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

struct t_object_base;

class t_select_clock
{
public:
  t_select_clock(t_canvas* canvas, t_object_base* obj);
  ~t_select_clock();

  static void deselect(t_select_clock* x);
private:
  t_clock* m_clock{};
  t_object_base* m_obj{};
  t_canvas* m_canvas{};

};

class t_matcher
{
public:
  t_matcher(ossia::net::node_base* n, t_object_base* p); // constructor
  ~t_matcher();
  t_matcher(const t_matcher&) = delete;
  t_matcher(t_matcher&& other);
  t_matcher& operator=(const t_matcher&) = delete;
  t_matcher& operator=(t_matcher&& other);

  void enqueue_value(const ossia::value& v);
  void output_value();
  auto get_node() const { return node; }
  auto get_parent() const { return parent; }
  void set_parent_addr();

  inline bool operator==(const t_matcher& rhs)
  { return (get_node() == rhs.node); }

private:
  ossia::net::node_base* node{};
  t_object_base* parent{};

  ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
    callbackit = ossia::none;

  moodycamel::ReaderWriterQueue<ossia::value, 64> m_queue_list;

  t_atom m_addr;

};

class t_object_base
{

public:
  t_object_base(t_eclass* c);

  t_eobj m_obj;

  t_outlet* m_setout{};
  t_outlet* m_dataout{};
  t_outlet* m_dumpout{};

  object_class m_otype{};
  t_symbol* m_name{};
  address_scope m_addr_scope{};
  bool m_is_pattern{}; // whether the address is a pattern or not
  bool m_dead{}; // whether this object is being deleted or not
  bool m_is_deleted{}; // true during the is_deleted callback method
  bool m_mute{};
  bool m_enable{};

  t_clock* m_clock{};   // multi-purpose clock
  t_clock* m_poll_clock{}; // value polling clock
  float m_poll_interval{10};
  std::chrono::milliseconds m_last_click{};

  ossia::optional<ossia::unit_t> m_ounit;

  ossia::net::generic_device* m_device{};
  std::vector<ossia::net::node_base*> m_nodes{};
  ossia::net::node_base* m_parent_node{};
  std::vector<t_matcher> m_matchers{};

  void is_deleted(const ossia::net::node_base& n);

  // TODO why some methods are inside t_obj_base class and other are outside ?
  static void push(t_object_base* x, t_symbol*, int argc, t_atom* argv);
  static void bang(t_object_base* x);
  static void output_value(t_object_base* x);

};

/**
 * @brief find_and_display_friend go through all registered parameters to find the ones that matches current remote
 * @param x the remote
 * @return false if nothing have been found
 */
bool find_and_display_friend(t_object_base* x);

/**
 * @brief obj_namespace send the namespace through dump outlet
 * @note only relevant for client, device, model and view objects.
 * @param x
 */
void obj_namespace(t_object_base* x);

/**
 * @brief obj_set
 * @param x
 * @param s
 * @param argc
 * @param argv
 */
void obj_set(t_object_base* x, t_symbol* s, int argc, t_atom* argv);

/**
 * @brief obj_get_address return global address through dump outlet
 * @param x
 */
void obj_get_address(t_object_base *x);

void obj_preset(t_object_base *x, t_symbol* s, int argc, t_atom* argv);
}
} // namespace
