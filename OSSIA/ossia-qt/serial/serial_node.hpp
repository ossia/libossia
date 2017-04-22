#pragma once
#include <ossia-qt/serial/serial_address.hpp>
#include <ossia/network/base/node.hpp>

namespace ossia
{
namespace net
{

class serial_device;
class serial_address;
class serial_node :
    public ossia::net::node_base
{
protected:
  std::string mName;
  serial_device& mDevice;
  serial_node* mParent{};
  std::unique_ptr<serial_address> mAddress;

public:
  serial_node(
      const serial_address_data& dat,
      serial_device& aDevice);

  serial_node(
      const serial_address_data& dat,
      serial_device& aDevice,
      serial_node& aParent);

  std::string getName() const override;

  ossia::net::device_base& get_device() const final override;

  ossia::net::node_base* get_parent() const final override;

  ossia::net::node_base& setName(std::string) override;

  ossia::net::address_base* getAddress() const final override;

  ossia::net::address_base* create_address(ossia::val_type) final override;

  bool removeAddress() final override;

  void add_child(std::unique_ptr<ossia::net::node_base>);

  std::unique_ptr<node_base> makeChild(const std::string& name) override
  { return nullptr; }
  void removingChild(ossia::net::node_base& node_base) override
  { }
};


}
}
