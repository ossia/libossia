#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>

namespace ossia
{
namespace net
{
generic_device::generic_device(
    std::unique_ptr<ossia::net::protocol_base> protocol, std::string name)
  : device_base(std::move(protocol)), generic_node(std::move(name), *this)
{
  mProtocol->setDevice(*this);
  mCapabilities.change_tree = true;
}

generic_device::~generic_device()
{
  removeAddress();
  write_lock_t lock{m_mutex};
  m_children.clear();
  mProtocol.reset();
}

}
}
