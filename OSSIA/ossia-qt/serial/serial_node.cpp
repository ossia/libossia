#include <ossia-qt/serial/serial_node.hpp>
#include <ossia-qt/serial/serial_device.hpp>
#include <ossia-qt/serial/serial_address.hpp>

namespace ossia
{
namespace net
{

serial_node::serial_node(
    const serial_address_data& data,
    serial_device& aDevice,
    serial_node& aParent):
  mName{data.node_name},
  mDevice{aDevice},
  mParent{&aParent}
{
  if(!data.request.isEmpty() || data.type)
    mAddress = std::make_unique<serial_address>(data, *this);
}

serial_node::serial_node(
    const serial_address_data& data,
    serial_device& aDevice):
  mName{data.node_name},
  mDevice{aDevice}
{
  if(!data.request.isEmpty() || data.type)
    mAddress = std::make_unique<serial_address>(data, *this);
}

std::string serial_node::getName() const
{
  return mName;
}

ossia::net::device_base& serial_node::getDevice() const
{
  return mDevice;
}

ossia::net::node_base* serial_node::getParent() const
{
  return mParent;
}

ossia::net::node_base& serial_node::setName(std::string)
{
  return *this;
}

ossia::net::address_base* serial_node::getAddress() const
{
  return mAddress.get();
}

ossia::net::address_base* serial_node::createAddress(ossia::val_type)
{
  return getAddress();
}

bool serial_node::removeAddress()
{
  return false;
}

void serial_node::add_child(std::unique_ptr<node_base> p)
{
  if(p)
  {
    lock_t lock{m_mutex};
    m_children.push_back(std::move(p));
  }
}

}
}
