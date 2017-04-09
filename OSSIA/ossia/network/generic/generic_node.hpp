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
  std::string mName;
  ossia::net::device_base& mDevice;
  ossia::net::node_base* mParent{};

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

  ossia::net::device_base& getDevice() const final override;
  ossia::net::node_base* getParent() const final override;
  std::string getName() const final override;
  ossia::net::node_base& setName(std::string) final override;
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

  ossia::net::address_base* getAddress() const final override;
  ossia::net::address_base* createAddress(ossia::val_type type) final override;
  void setAddress(std::unique_ptr<ossia::net::address_base> addr) final override;
  bool removeAddress() final override;

protected:
  std::unique_ptr<ossia::net::address_base> mAddress;

private:
  std::unique_ptr<node_base> makeChild(const std::string& name) final override;

  void removingChild(node_base&) final override
  {
  }
};
}
}
