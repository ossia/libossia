#pragma once

#include <ossia-max/src/parameter_base.hpp>
#include <ossia/detail/optional.hpp>

namespace ossia
{
namespace max
{

class attribute : public parameter_base
{
public:
  using is_attribute = std::true_type;

  void do_registration();
  void unregister();

  ossia::net::device_base* m_dev{};

  static void assist(attribute*, void*, long, long, char*);
  static t_max_err notify(attribute *x, t_symbol *s,
                          t_symbol *msg, void *sender, void *data);
  static void destroy(attribute* x);
  static void* create(t_symbol* name, int argc, t_atom* argv);

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);
  void on_device_deleted(const ossia::net::node_base&);
};
} // namespace pd
} // namespace ossia
