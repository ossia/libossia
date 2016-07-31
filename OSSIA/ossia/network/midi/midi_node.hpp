#pragma once

#include <ossia/network/base/node.hpp>

namespace ossia
{
namespace net
{
namespace midi
{
class midi_protocol;
class midi_device;
class OSSIA_EXPORT midi_node : public ossia::net::node_base
{
protected:
  midi_device& mDevice;
  node_base& mParent;
  std::unique_ptr<address_base> mAddress;

public:
  using iterator = ossia::ptr_container<ossia::net::node_base>::iterator;
  using const_iterator
      = ossia::ptr_container<ossia::net::node_base>::const_iterator;
  midi_node() = default;
  midi_node(midi_device& aDevice, ossia::net::node_base& aParent);

  device_base& getDevice() const final override;
  node_base* getParent() const final override;

  node_base& setName(std::string) override;

  address_base* getAddress() const final override;
  address_base* createAddress(val_type) final override;
  bool removeAddress() final override;

  std::unique_ptr<node_base> makeChild(const std::string& name) final override;
  void removingChild(node_base& node_base) final override;
};
}
}
}
