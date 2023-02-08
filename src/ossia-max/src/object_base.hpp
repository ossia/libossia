#pragma once
#include "ext.h"
#include "ext_critical.h"
#include "ext_obex.h"
#include "matcher.hpp"

#include <ossia/detail/config.hpp>

#include <ossia/detail/callback_container.hpp>
#include <ossia/detail/optional.hpp>
#include <ossia/detail/safe_vec.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/base/osc_address.hpp>
#include <ossia/network/base/value_callback.hpp>
#include <ossia/network/common/path.hpp>
#include <ossia/network/dataspace/dataspace.hpp>
#include <ossia/network/generic/generic_device.hpp>

#include <iostream>
#include <map>

#define OSSIA_MAX_MAX_ATTR_SIZE 256

namespace ossia
{
namespace max_binding
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
  attribute,
  explorer,
  search,
  monitor,
  fuzzysearch,
  cue
};

struct object_base;
class ossia_max;

struct search_result
{
  object_base* object{};
  t_object* patcher{};
  t_object* parent{};
  t_symbol* classname{};
  int level{}; // relative hierarchy level

  friend bool operator<(search_result a, search_result b) { return a.level < b.level; }
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

  // flags
  bool m_dead{false}; // whether this object is being deleted or not;
  bool m_is_deleted{};
  bool m_lock{false}; // attribute lock
  bool m_local_mute{false};
  bool m_registering{}; // true while registering to prevent recursive registration
  bool m_registered{};  // true if register_node() have been called at least once
  ossia::net::address_scope m_addr_scope{};
  object_class m_otype{};

  void* m_clock{};
  void* m_highlight_clock{}; // clock to reset color after some amount of time
                             // to highlight the object in the patcher

  float m_rate{10.f};

  std::shared_ptr<ossia::net::generic_device> m_device{};
  using matcher_vector = std::vector<std::shared_ptr<matcher>>;
  matcher_vector m_matchers{};
  std::vector<matcher*> m_node_selection{};
  std::optional<ossia::traversal::path> m_selection_path{};
  static void class_setup(t_class* c);

  void fill_selection();
  void update_path(ossia::net::generic_device* explicit_device = nullptr);
  void get_hierarchy();

  void set_description();
  void set_tags();
  void set_priority();
  void set_hidden();
  void set_recall_safe();

  // return the global path of the object with pattern
  object_base* find_parent_object();

  // return the first parent ossia object, nullptr otherwise
  object_base*
  find_parent_object_recursively(t_object* patcher, bool look_for_model_view);

  static void get_description(object_base* x, std::vector<matcher*> nodes);
  static void get_tags(object_base* x, std::vector<matcher*> nodes);
  static void get_priority(object_base* x, std::vector<matcher*> nodes);
  static void get_hidden(object_base* x, std::vector<matcher*> nodes);
  static void get_zombie(object_base* x, std::vector<matcher*> nodes);
  static void get_mess_cb(object_base* x, t_symbol* s);
  static void select_mess_cb(object_base* x, t_symbol* s, int argc, t_atom* argv);
  static void get_recall_safe(object_base* x, std::vector<matcher*> nodes);

  // default attributes
  t_symbol* m_name{};
  t_symbol* m_tags[OSSIA_MAX_MAX_ATTR_SIZE] = {{}};
  t_symbol* m_description{};
  float m_priority{};
  long m_invisible{};
  long m_defer_set{1};
  long m_recall_safe{};
  long m_trim_addr{1};
  long m_saveget{};
  long m_saveset{};
  long m_savebi{1};

  t_object* m_patcher{};

  long m_tags_size{};
  long m_description_size{};
  t_jrgba m_color{};

  std::vector<search_result> m_found_parameters{};
  std::vector<search_result> m_found_models{};

  // constructor
  object_base();

  // dtor
  ~object_base();

  bool m_loadbanged{}; // true if object received a loadbang

  std::mutex m_bind_mutex;
  // TODO check where this is filled
  std::vector<t_object*> m_patcher_hierarchy; // canvas hierarchy in ascending order, the
                                              // last is the root patcher

  static void update_attribute(
      object_base* x, ossia::string_view attribute, const ossia::net::node_base* node);
  static t_max_err
  notify(object_base* x, t_symbol* s, t_symbol* msg, void* sender, void* data);
  void on_parameter_removing(const ossia::net::parameter_base& n);
  void on_node_removing(const ossia::net::node_base& n);

  static void defer_set_output(object_base* x, t_symbol* s, int argc, t_atom* argv);
  static void set(object_base* x, t_symbol* s, int argc, t_atom* argv);
  static void get_address(object_base* x, std::vector<matcher*> nodes);
  static void lock_and_touch(object_base* x, t_symbol* s);
  static void loadbang(object_base* x);
  void save_children_state();
  void highlight();
  void clear_and_init_registration();

  static void reset_color(object_base* x);

  void push_parameter_value(ossia::net::parameter_base* param, const ossia::value& val);
  void set_matchers_index();

  std::vector<std::string> m_paths{};

  std::vector<std::shared_ptr<matcher>> find_or_create_matchers(ossia::net::generic_device* device_to_use = nullptr);

protected:
  void remove_matchers(const ossia::net::node_base& m);
  void remove_matcher(const std::shared_ptr<matcher>& m);

  [[nodiscard]] matcher_vector::iterator remove_matcher(matcher_vector::iterator m);

  std::map<std::string, ossia::value> m_value_map{};

private:
  std::vector<std::shared_ptr<matcher>> find_parent_nodes();
  void load_configuration(ossia_max& omax);
  void create_patcher_hierarchy(ossia_max& omax);
  unsigned long poly_index();
  void make_global_paths(const std::string& name);
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
    atom_setsym(&a, _sym_bang);
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

  void operator()(auto*) const noexcept = delete;

  void operator()(const char* str) const
  {
    t_symbol* s = gensym(str);
    t_atom a;
    atom_setsym(&a, s);
    data.push_back(a);
  }

  void operator()(std::string_view str) const
  {
    t_symbol* s = gensym(str.data());
    t_atom a;
    atom_setsym(&a, s);
    data.push_back(a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    data.reserve(data.size() + N);
    for(std::size_t i = 0; i < N; i++)
    {
      t_atom a;
      atom_setfloat(&a, vec[i]);
      data.push_back(a);
    }
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    data.reserve(data.size() + t.size());
    for(const auto& v : t)
      v.apply(*this);
  }

  void operator()(const ossia::value_map_type& t) const { }

  template <typename... T>
    requires(sizeof...(T) > 1)
  OSSIA_INLINE void operator()(T&&... t)
  {
    ((*this)(t), ...);
  }

  void operator()() const { }
};

inline void
write_message(std::vector<t_atom>& va, void* out, t_symbol* sym, auto&&... args)
{
  va.clear();
  value2atom vm{va};
  vm(args...);
  outlet_anything(out, sym, va.size(), va.data());
}

inline void write_message(
    std::vector<t_atom>& va, void* out, t_symbol* prefix, t_symbol* sym, auto&&... args)
{
  if(!prefix)
  {
    write_message(va, out, sym, args...);
  }
  else
  {
    va.clear();
    value2atom vm{va};
    vm(std::string_view(sym->s_name));
    vm(args...);
    outlet_anything(out, prefix, va.size(), va.data());
  }
}
// Template typed function switcher to convert t_atom or standard type into
// ossia::value
template <typename T>
struct value_visitor
{
  T* x;

  void set_out(t_atom& a) const
  {
    if(x->m_set_out)
    {
      if(x->m_defer_set)
      {
        defer_low((t_object*)x, (method)object_base::defer_set_output, _sym_set, 1, &a);
      }
      else
      {
        outlet_anything(x->m_set_out, _sym_set, 1, &a);
      }
    }
  }

  void set_out(int N, t_atom* a) const
  {
    if(x->m_set_out)
    {
      if(x->m_defer_set)
      {
        defer_low((t_object*)x, (method)object_base::defer_set_output, _sym_set, N, a);
      }
      else
      {
        outlet_anything(x->m_set_out, _sym_set, N, a);
      }
    }
  }

  void operator()(impulse) const
  {
    outlet_bang(x->m_data_out);

    if(x->m_set_out)
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
    {
      outlet_float(x->m_data_out, f);
    }
    set_out(a);
  }

  void operator()(bool b) const { (*this)(b ? 1 : 0); }

  void operator()(const std::string& str) const
  {
    t_symbol* s = gensym(str.c_str());
    t_atom a;

    atom_setsym(&a, s);
    outlet_anything(x->m_data_out, s, 0, nullptr);

    set_out(a);
  }

  template <std::size_t N>
  void operator()(std::array<float, N> vec) const
  {
    t_atom a[N];

    for(int i = 0; i < N; i++)
      atom_setfloat(a + i, vec[i]);
    outlet_list(x->m_data_out, _sym_list, N, a);

    set_out(N, a);
  }

  void operator()(const std::vector<ossia::value>& t) const
  {
    std::vector<t_atom> va;
    value2atom vm{va};

    if(t.empty())
      return;

    for(const auto& v : t)
      v.apply(vm);

    t_atom* list_ptr = !va.empty() ? va.data() : nullptr;
    if(x->m_data_out)
      outlet_list(x->m_data_out, _sym_list, va.size(), list_ptr);

    set_out(va.size(), list_ptr);
  }

  void operator()(const ossia::value_map_type& t) const { }

  void operator()() const
  {
    object_error((t_object*)x, "%s received an invalid data", x->m_name->s_name);
  }
};

} // max namespace
} // ossia namespace
