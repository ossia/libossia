#pragma once
#include <ossia-pd/src/node_base.hpp>

namespace ossia
{
namespace pd
{

class device_base : public node_base
{
public:
  device_base(t_eclass* c);

  void on_parameter_created_callback(const ossia::net::parameter_base& param);
  void on_parameter_deleted_callback(const ossia::net::parameter_base& param);
  static void on_attribute_modified_callback(ossia::net::node_base& node,
                                      const std::string& attribute);

  void connect_slots();
  void disconnect_slots();

};

} // namespace pd
} // namespace ossia
