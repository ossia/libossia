#pragma once
#include <ossia/network/base/node.hpp>
#include <QString>
#include <ossia-qt/http/http_parameter_data.hpp>

namespace ossia
{
namespace net
{
class http_device;
class http_parameter;
class OSSIA_EXPORT http_node
    : public ossia::net::node_base
{
  ossia::net::http_device& m_device;
  ossia::net::http_node* m_parent{};
  std::unique_ptr<ossia::net::http_parameter> m_parameter;

  friend class http_protocol;

public:
  http_node(
      const http_parameter_data& dat, ossia::net::http_device& aDevice,
      http_node& aParent);

  http_node(const http_parameter_data& dat, ossia::net::http_device& aDevice);

  ~http_node() override;

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
