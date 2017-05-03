#pragma once
#include <ossia-qt/http/http_address_data.hpp>
#include <ossia/network/base/node.hpp>
#include <QString>

namespace ossia
{
namespace net
{
class http_device;
class http_address;
class OSSIA_EXPORT http_node :
    public ossia::net::node_base
{
  std::string mName;
  ossia::net::http_device& mDevice;
  ossia::net::http_node* mParent{};
  std::unique_ptr<ossia::net::http_address> mAddress;

  friend class http_protocol;
public:
  http_node(
      const http_address_data& dat,
      ossia::net::http_device& aDevice,
      http_node& aParent);

  http_node(
      const http_address_data& dat,
      ossia::net::http_device& aDevice);

  ~http_node();

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
