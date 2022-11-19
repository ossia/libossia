#pragma once
#include "ext.h"
#include "ext_obex.h"

#include <rapidjson/stringbuffer.h>
#include <ossia/preset/cue.hpp>

namespace ossia
{
namespace max_binding
{

struct ocue
{
  t_object m_object;
  t_symbol* m_name;

  void* m_mainout{};
  void* m_dumpout{};

  void create(int argc, t_atom* argv);
  void update(int argc, t_atom* argv);
  void recall(int argc, t_atom* argv);
  void remove(int argc, t_atom* argv);
  void clear();
  void move(int argc, t_atom* argv);
  void output(int argc, t_atom* argv);

  void namespace_select(int argc, t_atom* argv);
  void namespace_deselect(int argc, t_atom* argv);
  void namespace_grab(int argc, t_atom* argv);

  static void in_long(ocue* x, long argc);
  static void reset(ocue* x);
  static void free(ocue* x);
  static void closebang(ocue* x);
  static t_max_err
  notify(ocue* x, t_symbol* s, t_symbol* msg, void* sender, void* data);

  static void assist(ocue* x, void* b, long m, long a, char* s);


  std::shared_ptr<ossia::cues> m_cues;
};

} // namespace max
} // namespace ossia
