#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <cassert>
#include <boost/algorithm/string/replace.hpp>
#include <iostream>
namespace ossia
{
namespace net
{
generic_node::generic_node(
    std::string name, ossia::net::device_base& aDevice, generic_node& aParent)
    : mName{std::move(name)}, mDevice{aDevice}, mParent{&aParent}
{
}

generic_node::generic_node(std::string name, ossia::net::device_base& aDevice)
    : mName{std::move(name)}, mDevice{aDevice}
{
}

generic_node::~generic_node()
{
  mChildren.clear();
}

ossia::net::node_base& generic_node::setName(std::string name)
{
  mName = sanitize_name(std::move(name));

  // notify observers
  mDevice.onNodeRenamed(*this, mName);

  return *this;
}

ossia::net::address_base* generic_node::getAddress() const
{
  return mAddress.get();
}

ossia::net::address_base* generic_node::createAddress(ossia::val_type type)
{
  // clear former address
  removeAddress();

  // edit new address
  mAddress = std::make_unique<ossia::net::generic_address>(*this);

  // set type
  mAddress->setValueType(type);

  // notify observers
  mDevice.onAddressCreated(*mAddress);

  return mAddress.get();
}

bool generic_node::removeAddress()
{
  // use the device protocol to stop address value observation
  if (mAddress)
  {
    // notify observers
    mDevice.onAddressRemoving(*mAddress);

    auto& device = getDevice();
    device.getProtocol().observe(*mAddress, false);

    mAddress.reset();

    return true;
  }

  return false;
}

std::unique_ptr<ossia::net::node_base>
generic_node::makeChild(const std::string& name_base)
{
  std::vector<std::string> brethren;
  brethren.reserve(mChildren.size());

  std::transform(mChildren.begin(), mChildren.end(), std::back_inserter(brethren),
                 [] (const auto& n) { return n->getName(); });

  return std::make_unique<generic_node>(
              sanitize_name(name_base, brethren),
              mDevice,
              *this);
}
}
}
