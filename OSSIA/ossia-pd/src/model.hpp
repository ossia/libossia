#pragma once

#include "device.hpp"
#include "ossia_obj_base.hpp"

namespace ossia
{
namespace pd
{

struct t_model : t_object_base
{
  using is_model = std::true_type;

  bool register_node(std::vector<ossia::net::node_base*> node);
  bool do_registration(std::vector<ossia::net::node_base*> node);
  bool unregister();
  void register_children();

  static ossia::safe_vector<t_model*>& quarantine();

  void is_deleted(const ossia::net::node_base& n);

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
