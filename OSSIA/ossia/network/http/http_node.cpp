#include <ossia/network/http/http_node.hpp>
#include <ossia/network/http/http_address.hpp>
#include <ossia/network/http/http_device.hpp>

namespace ossia
{
namespace net
{

http_node::http_node(
    const http_address_data& data,
    http_device& aDevice,
    http_node& aParent):
  mName{data.node_name},
  mDevice{aDevice},
  mParent{&aParent}
{
  if(!data.request.isEmpty() || data.type)
    mAddress = std::make_unique<http_address>(data, *this);
}

http_node::http_node(
    const http_address_data& data,
    http_device& aDevice):
  mName{data.node_name},
  mDevice{aDevice}
{
  if(!data.request.isEmpty() || data.type)
    mAddress = std::make_unique<http_address>(data, *this);
}

http_node::~http_node()
{

}

device_base& http_node::getDevice() const
{ return mDevice; }

node_base*http_node::getParent() const
{ return mParent; }

std::string http_node::getName() const
{ return mName; }

node_base&http_node::setName(std::string)
{ return *this; }

address_base* http_node::getAddress() const
{ return mAddress.get(); }

address_base*http_node::createAddress(val_type)
{ return nullptr; }

bool http_node::removeAddress()
{ return false; }

void http_node::add_child(std::unique_ptr<node_base> p)
{
  if(p)
  {
    mChildren.push_back(std::move(p));
  }
}

std::unique_ptr<node_base> http_node::makeChild(const std::string& name)
{ return nullptr; }

void http_node::removingChild(node_base& node_base)
{ }

}
}
