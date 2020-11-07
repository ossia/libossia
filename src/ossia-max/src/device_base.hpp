#pragma once
#include <ossia-max/src/node_base.hpp>

namespace ossia
{
namespace max
{

class device_base : public node_base
{
public:
  void on_parameter_created_callback(const ossia::net::parameter_base& param);
  void on_parameter_deleted_callback(const ossia::net::parameter_base& param);
  static void on_attribute_modified_callback(ossia::net::node_base& node,
                                      const std::string& attribute);
  void on_unhandled_message_callback(const std::string, const ossia::value& val);

  void on_node_renamed_callback(ossia::net::node_base& node,
                                      const std::string& old_name);
  void on_node_created_callback(ossia::net::node_base& node);
  void on_node_removing_callback(ossia::net::node_base& node);

  void connect_slots();
  void disconnect_slots();

  static   Nano::Signal<void(device_base*)> on_device_created; // The device being created
  static   Nano::Signal<void(device_base*)> on_device_removing; // The device being removed
};

} // namespace max
} // namespace ossia
