#pragma once
#include <ossia-pd/src/object_base.hpp>
#include <ossia/network/common/websocket_log_sink.hpp>

namespace ossia
{
namespace pd
{

class assert
{
public:
  t_eobj m_obj; // should be the first element
  t_atom m_atom[256]{};
  int m_size{};

  t_symbol* m_name;

  static void in_anything(ossia::pd::assert* x, t_symbol* s, int argc, t_atom* argv);
  static void destroy(ossia::pd::assert* x);
  static void* create(t_symbol* s, int argc, t_atom* argv);
  void quit();
};
} // pd namespace
} // ossia namespace

