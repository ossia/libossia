#pragma once
#include <ossia/network/base/node.hpp>
namespace ossia
{
class phidget_device;
class phidget_node : public ossia::net::node_base
{
protected:
  phidget_device& m_device;
  node_base& m_parent;
  std::unique_ptr<ossia::net::parameter_base> m_parameter;
public:
  using iterator = ossia::ptr_container<ossia::net::node_base>::iterator;
  using const_iterator
      = ossia::ptr_container<ossia::net::node_base>::const_iterator;

  ~phidget_node();

  void set_parameter(std::unique_ptr<ossia::net::parameter_base> a) override
  {
    m_parameter = std::move(a);
  }

  phidget_node(phidget_device& d, ossia::net::node_base& p);

  ossia::net::device_base& get_device() const final override;
  ossia::net::node_base* get_parent() const override;

  ossia::net::node_base& set_name(std::string n) override;

  ossia::net::parameter_base* get_parameter() const final override;
  ossia::net::parameter_base* create_parameter(val_type) final override;
  bool remove_parameter() final override;

  std::unique_ptr<ossia::net::node_base>
  make_child(const std::string& name) final override;
  void removing_child(ossia::net::node_base& node_base) final override;
};
}
