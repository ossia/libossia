#pragma once
#include <ossia/network/base/node.hpp>
#include <ossia/network/common/address_properties.hpp>
#include <ossia/detail/callback_container.hpp>
#include <functional>
#include <map>
namespace ossia
{
namespace net
{
class protocol_base;
class OSSIA_EXPORT generic_node :
    public ossia::net::node_base
{

protected:
  std::string mName;
  ossia::net::device_base& mDevice;
  generic_node* mParent{};
  std::unique_ptr<ossia::net::address_base> mAddress;

public:
  generic_node() = delete;
  generic_node(const generic_node&) = delete;
  generic_node(generic_node&&) = delete;
  generic_node& operator=(const generic_node&) = delete;
  generic_node& operator=(generic_node&&) = delete;

  generic_node(
      std::string name,
      ossia::net::device_base& aDevice,
      generic_node& aParent);

  generic_node(std::string name,
            ossia::net::device_base& aDevice);

  ~generic_node();

  ossia::net::device_base& getDevice() const final override { return mDevice; }
  ossia::net::node_base* getParent() const final override { return mParent; }

  std::string getName() const override { return mName; }
  ossia::net::node_base& setName(std::string) override;

  ossia::net::address_base* getAddress() const override;
  ossia::net::address_base* createAddress(ossia::val_type type) override;
  bool removeAddress() override;

private:
  std::unique_ptr<node_base> makeChild(const std::string& name) override;

  void removingChild(node_base& ) override
  {

  }
};

}
}
