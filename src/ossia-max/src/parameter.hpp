#pragma once

#include <ossia-max/src/parameter_base.hpp>

namespace ossia
{

namespace max_binding
{
class device;
class parameter final : public parameter_base
{
public:
  using is_parameter = std::true_type;

  void do_registration();
  bool unregister();
  void save_values();
  void on_device_created_callback(ossia::max_binding::device* dev);

  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max_binding::parameter*);

  static void assist(ossia::max_binding::parameter*, void*, long, long, char*);
  static t_max_err notify(
      ossia::max_binding::parameter* x, t_symbol* s, t_symbol* msg, void* sender,
      void* data);
  static void loadbang(parameter* x);
};

} // max namespace
} // ossia namespace
