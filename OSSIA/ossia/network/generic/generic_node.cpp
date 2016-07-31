#include <ossia/editor/value/value.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_address.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <cassert>

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
  std::swap(mName, name);

  // notify observers
  mDevice.onNodeRenamed(*this, name);

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
generic_node::makeChild(const std::string& name)
{
  // Find all the nodes that start with the same name.
  auto len = name.size();
  std::vector<int> instance_num;
  instance_num.reserve(mChildren.size());

  bool is_here = false;
  for (const auto& node : mChildren)
  {
    const std::string& n_name = node->getName();
    if (n_name == name)
    {
      is_here = true;
    }
    else
    {
      if (n_name.size() <= len)
        continue;

      if (n_name.compare(0, len, name) == 0 && n_name[len] == '.')
      {
        // Instance
        try
        {
          int n = std::stoi(n_name.substr(len));
          instance_num.push_back(n);
        }
        catch (...)
        {
          continue;
        }
      }
    }
  };

  if (!is_here)
  {
    return std::make_unique<generic_node>(name, mDevice, *this);
  }
  else
  {
    int n = instance_num.size();
    if (n == 0)
    {
      return std::make_unique<generic_node>(name + ".1", mDevice, *this);
    }
    else
    {
      // Find first number not in list
      std::sort(instance_num.begin(), instance_num.end());
      int i = 0;

      while (true)
      {
        if (i < n)
        {
          if ((1 + i) == instance_num[i])
          {
            i++;
          }
          else
          {
            return std::make_unique<generic_node>(
                name + "." + std::to_string(1 + i), mDevice, *this);
          }
        }
        else
        {
          return std::make_unique<generic_node>(
              name + "." + std::to_string(1 + i), mDevice, *this);
        }
      }
    }
  }
}
}
}
