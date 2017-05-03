#include <ossia-qt/http/http_node.hpp>
#include <ossia-qt/http/http_address.hpp>
#include <ossia-qt/http/http_device.hpp>

namespace ossia
{
namespace net
{

http_node::http_node(
    const http_address_data& data,
    http_device& aDevice,
    http_node& aParent):
  mName{data.name},
  mDevice{aDevice},
  mParent{&aParent}
{
  if(!data.request.isEmpty() || data.type)
    mAddress = std::make_unique<http_address>(data, *this);
}

http_node::http_node(
    const http_address_data& data,
    http_device& aDevice):
  mName{data.name},
  mDevice{aDevice}
{
  if(!data.request.isEmpty() || data.type)
    mAddress = std::make_unique<http_address>(data, *this);
}

http_node::~http_node()
{
  about_to_be_deleted(*this);

  write_lock_t lock{m_mutex};
  m_children.clear();
  mAddress.reset();
}

device_base& http_node::get_device() const
{ return mDevice; }

node_base*http_node::get_parent() const
{ return mParent; }

std::string http_node::get_name() const
{ return mName; }

node_base&http_node::set_name(std::string)
{ return *this; }

address_base* http_node::get_address() const
{ return mAddress.get(); }

address_base*http_node::create_address(val_type)
{ return nullptr; }

bool http_node::remove_address()
{ return false; }

void http_node::add_child(std::unique_ptr<node_base> p)
{
  if(p)
  {
    write_lock_t lock{m_mutex};
    m_children.push_back(std::move(p));
  }
}

std::unique_ptr<node_base> http_node::make_child(const std::string& name)
{ return nullptr; }

void http_node::removing_child(node_base& node_base)
{ }

}
}
