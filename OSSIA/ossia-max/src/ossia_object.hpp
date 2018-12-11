#pragma once
#include <ossia-max/src/device.hpp>

namespace ossia
{
namespace max
{

class ossia_object : public device
{
public:
  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia_object*);
  static t_max_err notify(ossia_object *x, t_symbol *s,
                         t_symbol *msg, void *sender, void *data);

  t_symbol* m_log_level;
};

} // max namespace
} // ossia namespace
