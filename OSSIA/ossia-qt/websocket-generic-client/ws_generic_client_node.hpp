#pragma once
#include <ossia-qt/websocket-generic-client/ws_generic_client_address_data.hpp>
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

  device_base& get_device() const override;
  node_base* get_parent() const override;

  std::string get_name() const override;
  node_base& set_name(std::string) override;

  address_base* get_address() const override;
  address_base* create_address(val_type = val_type::IMPULSE) override;
  bool remove_address() override;

  void add_child(std::unique_ptr<ossia::net::node_base>);
private:
  std::unique_ptr<node_base> make_child(const std::string& name) override;

  void removing_child(node_base& node_base) override;
};

}
}
