// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/network/base/device.hpp>
#include <ossia/network/base/protocol.hpp>

namespace ossia::net
{
device_base::~device_base() = default;

device_base::device_base(std::unique_ptr<protocol_base> proto)
    : m_protocol{std::move(proto)}
{
  m_capabilities.change_tree = true;
}

protocol_base& device_base::get_protocol() const
{
  return *m_protocol;
}

void device_base::apply_incoming_message(
    const message_origin_identifier& id, parameter_base& param, value&& value)
{
  auto v = param.set_value(std::move(value));
  on_message(param);
  if(m_echo)
    m_protocol->echo_incoming_message(id, param, v);
}

void device_base::apply_incoming_message_quiet(
    const message_origin_identifier& id, parameter_base& param, value&& value)
{
  auto v = param.set_value_quiet(std::move(value));
  on_message(param);
  if(m_echo)
    m_protocol->echo_incoming_message(id, param, v);
}
}
