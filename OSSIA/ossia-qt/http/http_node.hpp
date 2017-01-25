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
