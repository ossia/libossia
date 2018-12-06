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
                                      std::string& attribute);

  void connect_slots();
  void disconnect_slots();

};

} // namespace max
} // namespace ossia
