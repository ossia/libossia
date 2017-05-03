#pragma once
#include <ossia/detail/callback_container.hpp>
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <functional>
#include <map>
namespace ossia
{
namespace net
{
class protocol_base;

class OSSIA_EXPORT generic_node_base : public ossia::net::node_base
{
protected:
  std::string m_name;
  ossia::net::device_base& m_device;
  ossia::net::node_base* m_parent{};

public:
  generic_node_base() = delete;
  generic_node_base(const generic_node_base&) = delete;
  generic_node_base(generic_node_base&&) = delete;
  generic_node_base& operator=(const generic_node_base&) = delete;
  generic_node_base& operator=(generic_node_base&&) = delete;

  generic_node_base(
      std::string name,
      ossia::net::device_base& aDevice,
      node_base& aParent);

  generic_node_base(
      std::string name,
      ossia::net::device_base& aDevice);

  ossia::net::device_base& get_device() const final override;
  ossia::net::node_base* get_parent() const final override;
  std::string get_name() const final override;
  ossia::net::node_base& set_name(std::string) final override;
};

class OSSIA_EXPORT generic_node : public generic_node_base
{
public:
  generic_node(
      std::string name,
      ossia::net::device_base& aDevice,
      node_base& aParent);
  generic_node(
      std::string name,
      ossia::net::device_base& aDevice);

  ~generic_node();

  ossia::net::address_base* get_address() const final override;
  ossia::net::address_base* create_address(ossia::val_type type) final override;
  void set_address(std::unique_ptr<ossia::net::address_base> addr) final override;
  bool remove_address() final override;

protected:
  std::unique_ptr<ossia::net::address_base> m_address;

private:
  std::unique_ptr<node_base> make_child(const std::string& name) final override;

  void removing_child(node_base&) final override
  {
  }
};
}
}
