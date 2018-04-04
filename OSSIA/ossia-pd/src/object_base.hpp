#pragma once
#include <ossia/detail/safe_vec.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/network/base/value_callback.hpp>
#include <ossia/network/base/node_functions.hpp>
#include <ossia/detail/callback_container.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/common/path.hpp>

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
  attribute,
  param,
  remote,
  model,
  view,
  device,
  client
};

class object_base;

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

  void enqueue_value(ossia::value v);
  void output_value();
  auto get_node() const { return node; }
  auto get_owner() const { return owner; }
  t_atom* get_atom_addr_ptr() { return &m_addr; }
  void set_owner_addr();

  inline bool operator==(const t_matcher& rhs)
  { return (get_node() == rhs.node); }

  void set_dead(){ m_dead = true; }

  std::vector<ossia::value> m_set_pool;

private:
  ossia::net::node_base* node{};
  object_base* owner{};

  ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
    callbackit = ossia::none;

  moodycamel::ReaderWriterQueue<ossia::value, 64> m_queue_list;

  bool m_dead{};
  bool m_is_deleted{};
  t_atom m_addr{};
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
  ossia::net::address_scope m_addr_scope{};
  bool m_is_pattern{}; // whether the address is a pattern or not
  bool m_dead{false}; // whether this object is being deleted or not
  bool m_is_deleted{false}; // true during the is_deleted callback method
  unsigned int m_queue_length{64};
  int m_recall_safe{0};

  t_clock* m_clock{};   // multi-purpose clock
  std::chrono::milliseconds m_last_click{};

  t_clock* m_poll_clock{}; // value or message polling clock
  float m_rate{10};

  ossia::net::generic_device* m_device{};

  // TODO m_parent_nodes should be a vector
  // since we may use pattern matching in view and model
  ossia::net::node_base* m_parent_node{};
  std::vector<std::shared_ptr<t_matcher>> m_matchers{};
  std::vector<t_matcher*> m_node_selection{};
  ossia::optional<ossia::traversal::path> m_selection_path{};
  std::vector<t_canvas*> m_patcher_hierarchy; // canvas hierarchy in ascending order
                                              // starting at current canvas

  static void class_setup(t_eclass*c);

  void fill_selection();
  void update_path();

  void set_description();
  void set_tags();
  void set_priority();
  void set_hidden();
  void set_recall_safe();

  static void get_mess_cb(object_base* x, t_symbol* s);
  static t_pd_err notify(object_base*x, t_symbol*s, t_symbol* msg, void* sender, void* data);
  static void print_hierarchy(object_base* x);

  static void get_description(object_base* x, std::vector<t_matcher*> nodes);
  static void get_tags(object_base* x, std::vector<t_matcher*> nodes);
  static void get_priority(object_base* x, std::vector<t_matcher*> nodes);
  static void get_hidden(object_base* x, std::vector<t_matcher*> nodes);
  static void get_recall_safe(object_base* x, std::vector<t_matcher*> nodes);
  static void get_zombie(object_base*x, std::vector<t_matcher*> nodes);
  static void address_mess_cb(object_base* x, t_symbol* address);
  static void loadbang(object_base* x, t_float flag);

  t_atom m_tags[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  t_atom m_description[OSSIA_PD_MAX_ATTR_SIZE] = {{}};
  int m_priority{};
  bool m_hidden{};

  long m_tags_size{};
  long m_description_size{};

  // con/destructor
  object_base(t_eclass* c);
  ~object_base();


  static void update_attribute(object_base* x, ossia::string_view attribute, const ossia::net::node_base* node = nullptr);
  void is_deleted(const ossia::net::node_base& n);

  /**
   * @brief find_and_display_friend go through all registered parameters to find the ones that matches current remote
   * @param x the remote
   * @return false if nothing have been found
   */
  static bool find_and_display_friend(object_base* x);

  /**
   * @brief obj_get_address return global address through dump outlet
   * @param x
   */
  static void get_address(object_base *x, std::vector<t_matcher*> nodes);

  static void select_mess_cb(object_base* x, t_symbol* s, int argc, t_atom* argv);
protected:
  ossia::optional<ossia::traversal::path> m_path;
};

}
} // namespace
