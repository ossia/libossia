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
  std::string m_name;
  serial_device& m_device;
  serial_node* m_parent{};
  std::unique_ptr<serial_address> mAddress;

public:
  serial_node(
      const serial_address_data& dat,
      serial_device& aDevice);

  serial_node(
      const serial_address_data& dat,
      serial_device& aDevice,
      serial_node& aParent);

  std::string get_name() const override;

  ossia::net::device_base& get_device() const final override;

  ossia::net::node_base* get_parent() const final override;

  ossia::net::node_base& set_name(std::string) override;

  ossia::net::address_base* get_address() const final override;

  ossia::net::address_base* create_address(ossia::val_type) final override;

  bool remove_address() final override;

  void add_child(std::unique_ptr<ossia::net::node_base>);

  std::unique_ptr<node_base> make_child(const std::string& name) override
  { return nullptr; }
  void removing_child(ossia::net::node_base& node_base) override
  { }
};


}
}
