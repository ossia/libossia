#include "http_protocol.hpp"
namespace ossia
{
namespace net
{

http_node::http_node(http_address_data data, http_device& aDevice, http_node& aParent):
  mName{data.name},
  mDevice{aDevice},
  mParent{&aParent}
{
  if(!data.request.isEmpty())
    mAddress = std::make_unique<http_address>(data, *this);
}

http_node::http_node(http_address_data data, http_device& aDevice):
  mName{data.name},
  mDevice{aDevice}
{
  if(!data.request.isEmpty())
    mAddress = std::make_unique<http_address>(data, *this);
}

http_node::~http_node()
{

}

device_base&http_node::getDevice() const
{ return mDevice; }

node_base*http_node::getParent() const
{ return mParent; }

std::string http_node::getName() const
{ return mName.toStdString(); }

node_base&http_node::setName(std::string)
{ return *this; }

address_base* http_node::getAddress() const
{ return mAddress.get(); }

address_base*http_node::createAddress(val_type)
{ return nullptr; }

bool http_node::removeAddress()
{ return false; }

std::unique_ptr<node_base> http_node::makeChild(const std::string& name)
{ return nullptr; }

void http_node::removingChild(node_base& node_base)
{ }

http_address::http_address(http_address_data data, const node_base& node_base):
  generic_address{node_base},
  mData{data}
{

}

http_address::~http_address()
{
  callback_container<value_callback>::callbacks_clear();
}

http_device::~http_device()
{

}

http_protocol::http_protocol(std::string code)
{

}

http_protocol::~http_protocol()
{

}

bool http_protocol::update(node_base& node_base)
{
  return false;

}

bool http_protocol::pull(address_base& address_base)
{
  return false;

}

bool http_protocol::push(const address_base& address_base)
{
  return false;

}

bool http_protocol::observe(address_base& address_base, bool enable)
{
  return false;

}

}
}
