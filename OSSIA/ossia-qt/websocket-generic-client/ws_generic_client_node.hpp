#pragma once
#include <ossia/network/base/node.hpp>
#include <QString>
#include <ossia-qt/websocket-generic-client/ws_generic_client_parameter_data.hpp>

namespace ossia
{
namespace net
{
class ws_generic_client_device;
class ws_generic_client_parameter;
class OSSIA_EXPORT ws_generic_client_node
    : public ossia::net::node_base
{
  ossia::net::ws_generic_client_device& m_device;
  ossia::net::ws_generic_client_node* m_parent{};
  std::unique_ptr<ossia::net::ws_generic_client_parameter> m_parameter;

  friend class ws_generic_client_protocol;

public:
  ws_generic_client_node(
      const ws_generic_client_parameter_data& dat,
      ossia::net::ws_generic_client_device& aDevice,
      ws_generic_client_node& aParent);

  ws_generic_client_node(
      const ws_generic_client_parameter_data& dat,
      ossia::net::ws_generic_client_device& aDevice);

  ~ws_generic_client_node() override;

  device_base& get_device() const final override;
  node_base* get_parent() const final override;

  node_base& set_name(std::string) final override;

  parameter_base* get_parameter() const final override;
  parameter_base* create_parameter(val_type = val_type::IMPULSE) final override;
  bool remove_parameter() final override;

  void add_child(std::unique_ptr<ossia::net::node_base>);

private:
  std::unique_ptr<node_base> make_child(const std::string& name) final override;

  void removing_child(node_base& node_base) final override;
};
}
}
