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
generic_node_base::generic_node_base(
    std::string name, ossia::net::device_base& aDevice, node_base& aParent)
    : mName{std::move(name)}, mDevice{aDevice}, mParent{&aParent}
{
}

generic_node_base::generic_node_base(std::string name, ossia::net::device_base& aDevice)
    : mName{std::move(name)}, mDevice{aDevice}
{
}

device_base&generic_node_base::getDevice() const
{
  return mDevice;
}


node_base*generic_node_base::getParent() const
{
  return mParent;
}


std::string generic_node_base::getName() const
{
  return mName;
}


node_base& generic_node_base::setName(std::string name)
{
  auto old_name = std::move(mName);
  if(mParent)
  {
    const auto& bros = mParent->children();
    std::vector<std::string> bros_names;
    bros_names.reserve(bros.size());

    std::transform(bros.cbegin(), bros.cend(), std::back_inserter(bros_names),
                   [] (const auto& n) { return n->getName(); });

    mName = sanitize_name(std::move(name), bros_names);
  }
  else
  {
    mName = sanitize_name(std::move(name));
  }

  // notify observers
  mDevice.onNodeRenamed(*this, old_name);

  return *this;
}



generic_node::generic_node(
    std::string name,
    device_base& aDevice,
    node_base& aParent):
  generic_node_base{std::move(name), aDevice, aParent}
{

}

generic_node::generic_node(
    std::string name,
    device_base& aDevice):
  generic_node_base{std::move(name), aDevice}
{

}

generic_node::~generic_node()
{
  aboutToBeDeleted(*this);

  mChildren.clear();
  removeAddress();
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
  return std::make_unique<generic_node>(
              name_base,
              mDevice,
              *this);
}

}
}
