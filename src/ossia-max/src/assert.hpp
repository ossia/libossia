#pragma once
#include "ext.h"
#include "ext_obex.h"

#include <rapidjson/stringbuffer.h>

namespace ossia
{
namespace max_binding
{

struct oassert
{
  t_object m_object;
  t_symbol* m_name{};

  t_jrgba m_color;

  enum Status
  {
    NORUN = -1,
    FAIL,
    SUCCESS
  } m_status;

  void* m_dumpout{};

  void update_color();
  void save_color();
  rapidjson::StringBuffer create_report();

  static void in_long(oassert* x, long argc);
  static void reset(oassert* x);
  static void free(oassert* x);
  static void closebang(oassert* x);
  static t_max_err
  notify(oassert* x, t_symbol* s, t_symbol* msg, void* sender, void* data);

  static void assist(oassert* x, void* b, long m, long a, char* s);
};

} // namespace max
} // namespace ossia

extern "C" {
void* ossia_assert_create(t_symbol* s, long argc, t_atom* argv);
}
