#pragma once
#include "ext.h"
#include "ext_obex.h"

#include <ossia/preset/cue.hpp>

#include <ossia-max/src/object_base.hpp>

#include <rapidjson/stringbuffer.h>

namespace ossia
{
namespace max_binding
{

struct ocue : object_base
{
  t_symbol* m_device_name = _sym_nothing;
  void* m_mainout{};

  void create(int argc, t_atom* argv);
  void update(int argc, t_atom* argv);
  void recall(int argc, t_atom* argv);
  void remove(int argc, t_atom* argv);
  void clear();
  void move(int argc, t_atom* argv);
  void output(int argc, t_atom* argv);
  void read(int argc, t_atom* argv);
  void write(int argc, t_atom* argv);
  void edit(int argc, t_atom* argv);
  void sort(int argc, t_atom* argv);
  void dump_all_cues();
  void dump_selection();

  void namespace_select(int argc, t_atom* argv);
  void namespace_filter_all(int argc, t_atom* argv);
  void namespace_filter_any(int argc, t_atom* argv);
  void namespace_deselect(int argc, t_atom* argv);
  void namespace_grab(int argc, t_atom* argv);

  void do_registration();
  void unregister();

  ossia::net::device_base* get_device() const noexcept;
  t_max_err get_device_name(long* ac, t_atom** av);
  t_max_err set_device_name(long ac, t_atom* av);

  static void in_long(ocue* x, long argc);
  static void reset(ocue* x);
  static void free(ocue* x);
  static void closebang(ocue* x);
  static t_max_err notify(ocue* x, t_symbol* s, t_symbol* msg, void* sender, void* data);

  static void assist(ocue* x, void* b, long m, long a, char* s);

  std::shared_ptr<ossia::cues> m_cues;
};

} // namespace max
} // namespace ossia
