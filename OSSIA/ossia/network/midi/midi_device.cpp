// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "midi_device.hpp"

#include <ossia/detail/logger.hpp>
#include <ossia/network/midi/detail/midi_impl.hpp>
#include <ossia/network/midi/midi_protocol.hpp>

namespace ossia::net::midi
{
midi_device::midi_device(std::unique_ptr<protocol_base> prot)
    : ossia::net::device_base{std::move(prot)}, midi_node{*this}
{
  m_protocol->set_device(*this);
}

midi_device::~midi_device()
{
  m_children.clear();

  about_to_be_deleted(*this);
}

node_base& midi_device::set_name(std::string n)
{
  m_name = n;
  return *this;
}

bool midi_device::create_full_tree()
{
  clear_children();

  // TODO set type list and send as raw bytes
  m_parameter = std::make_unique<midi_parameter>(
      address_info{{}, address_info::Type::Any, {}}, *this);

  try
  {
    for (int i = 1; i <= 16; i++)
    {
      auto ptr = std::make_unique<channel_node>(true, i, *this, *this);

      write_lock_t lock{m_mutex};
      m_children.push_back(std::move(ptr));
    }
  }
  catch (std::exception& e)
  {
    logger().error("midi_device::updateNamespace() catched: {}", e.what());
  }
  catch (...)
  {
    logger().error("midi_device::updateNamespace() failed.");
    return false;
  }
  return true;
}
}
