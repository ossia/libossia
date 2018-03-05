#pragma once

#include <ossia-pd/src/device.hpp>
#include <ossia-pd/src/parameter_base.hpp>

namespace ossia
{
namespace pd
{

struct view : public node_base
{
public:
  view();

  using is_view = std::true_type;

  bool register_node(const std::vector<t_matcher>& node);
  bool do_registration(const std::vector<t_matcher>& node);
  bool unregister();

  static ossia::safe_set<view*>& quarantine();

  static void* create(t_symbol* name, int argc, t_atom* argv);
  static void destroy(view* x);
  static void click(view* x, t_floatarg xpos, t_floatarg ypos,
    t_floatarg shift, t_floatarg ctrl, t_floatarg alt);

};
}
} // namespace
