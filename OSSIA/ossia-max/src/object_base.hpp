#pragma once
#include <ossia/detail/config.hpp>
#include "ext.h"

#include <ossia/detail/safe_vec.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/network/base/value_callback.hpp>
#include <ossia/detail/callback_container.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/common/path.hpp>

#include <concurrentqueue.h>


#define OSSIA_MAX_MAX_ATTR_SIZE 256

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_object_base structure declaration

enum class object_class
{
  root = 0,
  param,
  remote,
  model,
  view,
  device,
  client,
  attribute
};

struct object_base;

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
  ossia::net::node_base* get_node() const { return node; }
  object_base* get_parent() const { return owner; }
  t_atom* get_atom_addr_ptr() { return &m_addr; }
  void set_parent_addr();

  inline bool operator==(const t_matcher& rhs)
  { return (get_node() == rhs.node); }

  void set_dead(){ m_dead = true; }

  std::vector<ossia::value> m_set_pool;

private:
  ossia::net::node_base* node{};
  object_base* owner{};

  ossia::optional<ossia::callback_container<ossia::value_callback>::iterator>
    callbackit = ossia::none;

  moodycamel::ConcurrentQueue<ossia::value> m_queue_list;

  bool m_dead{};
  t_atom m_addr{};
};

struct search_result
{
  object_base* object{};
  t_object* patcher{};
  t_object* parent{};
  t_symbol* classname{};
  int level; // relative hierarchy level

  friend bool operator<(search_result a, search_result b)
  {
    return a.level < b.level;
  }
};

struct object_base
{
public:
  t_object m_object; // the Max object instance.
  // !!! this member is handled by Max API : that's why there is no place in
  // our code where it is initialized.

  void** m_inlets{};
  void* m_data_out{};
  void* m_set_out{};
  void* m_dumpout{};

  //flags
  bool m_is_pattern{};
  bool m_dead{false}; // wether this object is being deleted or not;
  bool m_is_deleted{};
  bool m_lock{false};
  long m_queue_length{64};
  ossia::net::address_scope m_addr_scope{};
  object_class m_otype{};

  void* m_clock{};
  void* m_poll_clock{}; // value or message polling clock

  float m_rate{10};

  ossia::net::generic_device* m_device{};
  // std::vector<ossia::net::node_base*> m_nodes{};
  ossia::net::node_base* m_parent_node{};
  std::vector<std::shared_ptr<t_matcher>> m_matchers{};
  std::vector<t_matcher*> m_node_selection{};
  ossia::optional<ossia::traversal::path> m_selection_path{};

  static void class_setup(t_class*c);

  void fill_selection();
  void update_path();
  void get_hierarchy();

  void set_description();
  void set_tags();
  void set_priority();
  void set_hidden();
  void set_recall_safe();

  static void get_description(object_base* x, std::vector<t_matcher*> nodes);
  static void get_tags(object_base* x, std::vector<t_matcher*> nodes);
  static void get_priority(object_base* x, std::vector<t_matcher*> nodes);
  static void get_hidden(object_base* x, std::vector<t_matcher*> nodes);
  static void get_zombie(object_base*x, std::vector<t_matcher*> nodes);
  static void get_mess_cb(object_base* x, t_symbol* s);
  static void select_mess_cb(object_base* x, t_symbol* s, int argc, t_atom* argv);
  static void get_recall_safe(object_base*x, std::vector<t_matcher*> nodes);


  // default attributes
  t_symbol* m_name{};
  t_symbol* m_tags[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_symbol* m_description{};
  long m_priority{};
  long m_invisible{};
  long m_defer_set{1};
  long m_recall_safe{};

  long m_tags_size{};
  long m_description_size{};

  std::vector<search_result> m_found_parameters{};
  std::vector<search_result> m_found_models{};
  t_object* m_patcher{};

  // constructor
  object_base();

  std::mutex bindMutex;
  std::vector<t_object*> m_patcher_hierarchy; // canvas hierarchy in ascending order


  static void update_attribute(object_base* x, ossia::string_view attribute, const ossia::net::node_base* node);
  static t_max_err notify(object_base *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
  void is_deleted(const ossia::net::node_base& n);

  static void defer_set_output(object_base*x, t_symbol*s ,int argc, t_atom* argv);
  static void set(object_base* x, t_symbol* s, int argc, t_atom* argv);
  static void get_address(object_base *x,  std::vector<t_matcher*> nodes);
  static void lock_and_touch(object_base* x, t_symbol* s);
  static void loadbang(object_base* x);

protected:
  ossia::optional<ossia::traversal::path> m_path;
};

#pragma mark -
#pragma mark Utilities

// Typed function switcher to convert ossia::value to t_atom
struct value2atom
{
  std::vector<t_atom>& data;
  void operator()(impulse) const
  {
    t_atom a;
    atom_setsym(&a, gensym("bang"));
    data.push_back(a);
  }

  void operator()(int32_t i) const
  {
    t_atom a;
    atom_setlong(&a, i);
    data.push_back(a);
  }

  void operator()(int64_t i) const
  {
    t_atom a;
    atom_setlong(&a, i);
    data.push_back(a);
  }

  void operator()(float f) const
  {
    t_atom a;
    atom_setfloat(&a, f);
    data.push_back(a);
  }

  void operator()(bool b) const
  {
    t_atom a;
    float f = b ? 1. : 0.;
    atom_setfloat(&a, f);
    data.push_back(a);
  }

  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;
    atom_setsym(&a, s);
    data.push_back(a);
  }

  void operator()(char c) const
  {
    t_atom a;
    atom_setfloat(&a, (float)c);
    data.push_back(a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    data.reserve(data.size() + N);
    for (std::size_t i = 0; i < N; i++)
    {
      t_atom a;
      atom_setfloat(&a, vec[i]);
      data.push_back(a);
    }
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    data.reserve(data.size() + t.size());
    for (const auto& v : t)
      v.apply(*this);
  }

  void operator()() const
  {
  }
};

// Template typed function switcher to convert t_atom or standard type into
// ossia::value
template <typename T>
struct value_visitor
{
  T* x;

  void set_out(t_atom& a) const
  {
    if (x->m_set_out)
    {
      if(x->m_defer_set)
      {
        defer_low((t_object*)x,(method)object_base::defer_set_output,
                  gensym("set"), 1, &a);
      }
      else
      {
        outlet_anything(x->m_set_out, gensym("set"), 1, &a);
      }
    }
  }

  void set_out(int N, t_atom* a) const
  {
    if (x->m_set_out)
    {
      if(x->m_defer_set)
      {
        defer_low((t_object*)x,(method)object_base::defer_set_output,
                  gensym("set"), N, a);
      }
      else
      {
        outlet_anything(x->m_set_out, gensym("set"), N, a);
      }
    }
  }

  void operator()(impulse) const
  {
    outlet_bang(x->m_data_out);

    if (x->m_set_out)
      outlet_bang(x->m_set_out);
  }

  void operator()(int32_t i) const
  {
    t_atom a;
    atom_setlong(&a, i);
    outlet_int(x->m_data_out, i);

    set_out(a);
  }

  void operator()(float f) const
  {
    t_atom a;

    atom_setfloat(&a, f);
    if(x && x->m_data_out)
      outlet_float(x->m_data_out, f);

    set_out(a);
  }

  void operator()(bool b) const
  {
    (*this)(b ? 1 : 0);
  }

  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;

    atom_setsym(&a, s);
    outlet_anything(x->m_data_out, s, 0, nullptr);

    set_out(a);
  }

  void operator()(char c) const
  {
    t_atom a;

    atom_setlong(&a, (long)c);
    outlet_int(x->m_data_out, (long)c);

    set_out(a);
  }

  template<std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    t_atom a[N];

    for(int i = 0; i < N; i++)
      atom_setfloat(a + i, vec[i]);
    outlet_list(x->m_data_out, gensym("list"), N, a);

    set_out(N, a);
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    std::vector<t_atom> va;
    value2atom vm{va};

    if(t.empty())
      return;

    for (const auto& v : t)
      v.apply(vm);

    t_atom* list_ptr = !va.empty() ? va.data() : nullptr;
    if (x->m_data_out)
      outlet_list(x->m_data_out, gensym("list"), va.size(), list_ptr);

    set_out(va.size(), list_ptr);
  }

  void operator()() const
  {
    object_error(
        (t_object*)x, "%s received an invalid data", x->m_name->s_name);
  }
};

} // max namespace
} // ossia namespace
