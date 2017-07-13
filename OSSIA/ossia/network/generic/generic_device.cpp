// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/network/base/protocol.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_node.hpp>
#include <ossia/network/local/local.hpp>

namespace ossia
{
namespace net
{
generic_device::generic_device()
    : device_base{std::make_unique<multiplex_protocol>()}
    , generic_node{"", *this}
{
  m_protocol->set_device(*this);
  m_capabilities.change_tree = true;
}

generic_device::generic_device(
    std::unique_ptr<ossia::net::protocol_base> protocol, std::string name)
    : device_base(std::move(protocol)), generic_node(std::move(name), *this)
{
  m_protocol->set_device(*this);
  m_capabilities.change_tree = true;
}

generic_device::~generic_device()
{
  remove_address();
  write_lock_t lock{m_mutex};
  m_children.clear();
  m_protocol.reset();
}
}
}
