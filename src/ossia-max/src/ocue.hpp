#pragma once
#include "ext.h"
#include "ext_obex.h"

#include <ossia/preset/cue.hpp>

#include <ossia-max/src/object_base.hpp>

#include <ossia-max/src/search_filter.hpp>
#include <rapidjson/stringbuffer.h>

namespace ossia
{
namespace max_binding
{

struct ocue 
  : object_base
  , search_sort_filter
{
  static t_class* max_class;
  t_symbol* m_device_name = _sym_nothing;

  void create(int argc, t_atom* argv);
  void update(int argc, t_atom* argv);
  void remove(int argc, t_atom* argv);
  void rename(int argc, t_atom* argv);
  void clear();
  void move(int argc, t_atom* argv);
  void output(int argc, t_atom* argv);
  void read(int argc, t_atom* argv);
  void write(int argc, t_atom* argv);
  void edit(int argc, t_atom* argv);
  void sort(int argc, t_atom* argv);
  void json();
  void names();

  void explore(int argc, t_atom* argv);
  void do_explore(t_symbol* name);

  void recall(int argc, t_atom* argv);
  void recall_next(int argc, t_atom* argv);
  void recall_previous(int argc, t_atom* argv);

  void display_model(int argc, t_atom* argv);
  void do_display_model(std::string_view);
  void select_model(int argc, t_atom* argv);
  void select(int argc, t_atom* argv);

  // Selection: what is actually going to be on the cue when we create / update
  void selection_dump();
  void selection_add(int argc, t_atom* argv);
  void selection_switch(int argc, t_atom* argv);
  void selection_filter_all(int argc, t_atom* argv);
  void selection_filter_any(int argc, t_atom* argv);
  void selection_remove(int argc, t_atom* argv);
  void selection_grab(int argc, t_atom* argv);

  // Filter: what we display as the output of ossia.explore
  void filter_dump();
  void filter_add(int argc, t_atom* argv);
  void filter_filter_all(int argc, t_atom* argv);
  void filter_filter_any(int argc, t_atom* argv);
  void filter_remove(int argc, t_atom* argv);
  void filter_grab(int argc, t_atom* argv);

  void do_registration();
  void unregister();

  void update_selection();

  ossia::net::device_base* get_device() const noexcept;
  t_max_err get_device_name(long* ac, t_atom** av);
  t_max_err set_device_name(long ac, t_atom* av);

  static void in_long(ocue* x, long argc);
  static void reset(ocue* x);
  static void free(ocue* x);
  static void closebang(ocue* x);
  static t_max_err notify(ocue* x, t_symbol* s, t_symbol* msg, void* sender, void* data);

  static void assist(ocue* x, void* b, long m, long a, char* s);

  template <typename... T>
  void dump_message(std::string_view str, T&&...);
  void dump_message(std::string_view msg, const std::vector<std::string_view>& t);

  std::shared_ptr<ossia::cues> m_cues;
  namespace_selection m_ns;
  namespace_selection m_filter;
  std::string m_last_filename;
  t_symbol* m_last_explore{_sym_nothing};
};

} // namespace max
} // namespace ossia
