#include <ossia/network/websocket-generic-client/ws_generic_client_node.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_address.hpp>
#include <ossia/network/websocket-generic-client/ws_generic_client_device.hpp>

namespace ossia
{
namespace net
{

ws_generic_client_node::ws_generic_client_node(
    const ws_generic_client_address_data& data,
    ws_generic_client_device& aDevice,
    ws_generic_client_node& aParent):
  mName{data.node_name},
  mDevice{aDevice},
  mParent{&aParent}
{
  if(!data.request.isNull() || data.type)
    mAddress = std::make_unique<ws_generic_client_address>(data, *this);
}

ws_generic_client_node::ws_generic_client_node(
    const ws_generic_client_address_data& data,
    ws_generic_client_device& aDevice):
  mName{data.node_name},
  mDevice{aDevice}
{
  if(!data.request.isNull() || data.type)
    mAddress = std::make_unique<ws_generic_client_address>(data, *this);
}

ws_generic_client_node::~ws_generic_client_node()
{

}

device_base& ws_generic_client_node::getDevice() const
{ return mDevice; }

node_base*ws_generic_client_node::getParent() const
{ return mParent; }

std::string ws_generic_client_node::getName() const
{ return mName; }

node_base&ws_generic_client_node::setName(std::string)
{ return *this; }

address_base* ws_generic_client_node::getAddress() const
{ return mAddress.get(); }

address_base*ws_generic_client_node::createAddress(val_type)
{ return nullptr; }

bool ws_generic_client_node::removeAddress()
{ return false; }

void ws_generic_client_node::add_child(std::unique_ptr<node_base> p)
{
  if(p)
  {
    mChildren.push_back(std::move(p));
  }
}

std::unique_ptr<node_base> ws_generic_client_node::makeChild(const std::string& name)
{ return nullptr; }

void ws_generic_client_node::removingChild(node_base& node_base)
{ }

}
}
