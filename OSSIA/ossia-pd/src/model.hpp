#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

class model : public t_object_base
{
public:
  using is_model = std::true_type;
  model();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();
  void register_children();

  static ossia::safe_vector<model*>& quarantine();

  void set_tags();
  void set_description();
  void set_priority();

  t_atom m_tags[OSSIA_PD_MAX_ATTR_SIZE];
  t_atom m_description[OSSIA_PD_MAX_ATTR_SIZE];
  int m_priority;

  long m_tags_size;
  long m_description_size;
};
}
} // namespace
