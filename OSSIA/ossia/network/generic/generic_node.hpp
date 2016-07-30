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
class protocol;
}
}
namespace impl
{
class OSSIA_EXPORT BasicNode :
    public ossia::net::node
{

protected:
  std::string mName;
  ossia::net::device& mDevice;
  BasicNode* mParent{};
  std::unique_ptr<ossia::net::address> mAddress;

public:
  BasicNode() = delete;
  BasicNode(const BasicNode&) = delete;
  BasicNode(BasicNode&&) = delete;
  BasicNode& operator=(const BasicNode&) = delete;
  BasicNode& operator=(BasicNode&&) = delete;

  BasicNode(
      std::string name,
      ossia::net::device& aDevice,
      BasicNode& aParent);

  BasicNode(std::string name,
            ossia::net::device& aDevice);

  ~BasicNode();

  ossia::net::device& getDevice() const final override { return mDevice; }
  ossia::net::node* getParent() const final override { return mParent; }

  std::string getName() const override { return mName; }
  ossia::net::node& setName(std::string) override;

  ossia::net::address* getAddress() const override;
  ossia::net::address* createAddress(ossia::Type type) override;
  bool removeAddress() override;

private:
  std::unique_ptr<node> makeChild(const std::string& name) override;

  void removingChild(node& node) override
  {

  }
};

}
