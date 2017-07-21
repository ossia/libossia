#pragma once
#include <ossia/network/base/device.hpp>
#include <ossia/network/phidgets/phidgets_node.hpp>

namespace ossia
{
class phidget_protocol;
class OSSIA_EXPORT phidget_device final : public ossia::net::device_base,
                                          public phidget_node
{
  std::string m_name;

public:
  phidget_device(std::string n);
  ~phidget_device();

  ossia::net::node_base* get_parent() const final override;

  const ossia::net::node_base& get_root_node() const override;
  ossia::net::node_base& get_root_node() override;

  phidget_protocol& get_protocol() const;
};
}
