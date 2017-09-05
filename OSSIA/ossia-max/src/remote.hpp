#pragma once
#include <ossia-max/src/parameter_base.hpp>
#include <boost/optional.hpp>

namespace ossia
{
namespace max
{

#pragma mark -
#pragma mark t_remote structure declaration

class remote : public parameter_base
{
public:
  remote();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  ossia::net::device_base* m_dev{};
  t_symbol* m_unit;
  float m_rate_min;

  void set_unit();
  void set_rate();

  void on_parameter_created_callback(const ossia::net::parameter_base& addr);

  static void update_attribute(remote* x, ossia::string_view attribute);
  static void bind(remote* x, t_symbol* address);
  //static void click( remote* x, t_floatarg xpos, t_floatarg ypos,
  //                   t_floatarg shift, t_floatarg ctrl, t_floatarg alt);
  static t_max_err notify(remote*x, t_symbol*s, t_symbol* msg, void* sender, void* data);

  //static void destroy(remote* x);
  //static void* create(t_symbol* name, int argc, t_atom* argv);

  static ossia::safe_vector<remote*>& quarantine();

  static void get_unit(remote*x);
  static void get_mute(remote*x);
  static void get_rate(remote*x);
  static void get_enable(remote*x);

  static void assist(ossia::max::remote*, void*, long, long, char*);
  static void* create(t_symbol*, long, t_atom*);
  static void destroy(ossia::max::remote*);

};

} // max namespace
} // ossia namespace
