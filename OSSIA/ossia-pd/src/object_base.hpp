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

struct object_base;

class t_select_clock
{
public:
  t_select_clock(t_canvas* canvas, object_base* obj);
  ~t_select_clock();

  static void deselect(t_select_clock* x);
private:
  t_clock* m_clock{};
  object_base* m_obj{};
  t_canvas* m_canvas{};

};

class t_matcher
{
public:
  t_matcher(ossia::net::node_base* n, object_base* p); // constructor
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

  void set_dead(){ m_dead = true; };

  bool m_dead{};

private:
  ossia::net::node_base* node{};
  object_base* parent{};

  ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
    callbackit = ossia::none;

  moodycamel::ReaderWriterQueue<ossia::value, 64> m_queue_list;

  t_atom m_addr;

};

class object_base
{

public:
  t_eobj m_obj; // should be the first element

  t_outlet* m_setout{};
  t_outlet* m_dataout{};
  t_outlet* m_dumpout{};

  object_class m_otype{};
  t_symbol* m_name{};
  address_scope m_addr_scope{};
  bool m_is_pattern{}; // whether the address is a pattern or not
  bool m_dead{false}; // whether this object is being deleted or not
  bool m_is_deleted{false}; // true during the is_deleted callback method
  bool m_mute{false};
  bool m_enable{true};

  t_clock* m_clock{};   // multi-purpose clock
  std::chrono::milliseconds m_last_click{};

  t_clock* m_poll_clock{}; // value or message polling clock
  float m_rate{10};

  ossia::net::generic_device* m_device{};
  std::vector<ossia::net::node_base*> m_nodes{};
  // TODO m_parent_nodes should be a vector
  // since we may use pattern matching in view and model
  ossia::net::node_base* m_parent_node{};
  std::vector<t_matcher> m_matchers{};

  static void declare_attributes(t_eclass*c);

  void set_description();
  void set_tags();
  void set_priority();
  void set_hidden();

  static void get_description(object_base* x);
  static void get_tags(object_base* x);
  static void get_priority(object_base* x);
  static void get_hidden(object_base* x);

  t_atom m_tags[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_description[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  int m_priority{};
  bool m_hidden{};

  long m_tags_size{};
  long m_description_size{};

  // constructor
  object_base(t_eclass* c);


  static void update_attribute(object_base* x, ossia::string_view attribute);
  void is_deleted(const ossia::net::node_base& n);

  /**
   * @brief find_and_display_friend go through all registered parameters to find the ones that matches current remote
   * @param x the remote
   * @return false if nothing have been found
   */
  static bool find_and_display_friend(object_base* x);

  /**
   * @brief set Set the parameter value from Pd patcher
   * @param x
   * @param s
   * @param argc
   * @param argv
   */
  static void set(object_base* x, t_symbol* s, int argc, t_atom* argv);

  /**
   * @brief obj_get_address return global address through dump outlet
   * @param x
   */
  static void get_address(object_base *x);

};

}
} // namespace
