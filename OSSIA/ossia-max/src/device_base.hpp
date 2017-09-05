#pragma once
#include <ossia-max/src/node_base.hpp>

namespace ossia
{
namespace max
{

class device_base : public node_base
{
public:
  device_base(t_class* c);

  void on_parameter_created_callback(const ossia::net::parameter_base& param);
  void on_parameter_deleted_callback(const ossia::net::parameter_base& param);
  void on_attribute_modified_callback(const ossia::net::node_base& node,
                                      ossia::string_view attribute);

  void connect_slots();
  void disconnect_slots();

};

} // namespace max
} // namespace ossia
