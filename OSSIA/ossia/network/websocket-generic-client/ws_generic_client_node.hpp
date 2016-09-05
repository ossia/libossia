#pragma once
#include <ossia/network/websocket-generic-client/ws_generic_client_address_data.hpp>
#include <ossia/network/base/node.hpp>
#include <QString>

namespace ossia
{
namespace net
{
class ws_generic_client_device;
class ws_generic_client_address;
class OSSIA_EXPORT ws_generic_client_node :
    public ossia::net::node_base
{
  std::string mName;
  ossia::net::ws_generic_client_device& mDevice;
  ossia::net::ws_generic_client_node* mParent{};
  std::unique_ptr<ossia::net::ws_generic_client_address> mAddress;

  friend class ws_generic_client_protocol;
public:
  ws_generic_client_node(
      const ws_generic_client_address_data& dat,
      ossia::net::ws_generic_client_device& aDevice,
      ws_generic_client_node& aParent);

  ws_generic_client_node(
      const ws_generic_client_address_data& dat,
      ossia::net::ws_generic_client_device& aDevice);

  ~ws_generic_client_node();

  device_base& getDevice() const override;
  node_base* getParent() const override;

  std::string getName() const override;
  node_base& setName(std::string) override;

  address_base* getAddress() const override;
  address_base* createAddress(val_type = val_type::IMPULSE) override;
  bool removeAddress() override;

  void add_child(std::unique_ptr<ossia::net::node_base>);
private:
  std::unique_ptr<node_base> makeChild(const std::string& name) override;

  void removingChild(node_base& node_base) override;
};

}
}
