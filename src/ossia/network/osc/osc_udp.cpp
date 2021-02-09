// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia/network/osc/osc_udp.hpp"

#include <ossia/detail/logger.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/osc/detail/osc_protocol_common.hpp>

namespace ossia::net
{
struct osc_udp_protocol::impl
{
  udp_socket from_client;
  udp_socket to_client;
};

osc_udp_protocol::osc_udp_protocol(
    network_context_ptr ctx,
    std::string_view local_host,  uint16_t local_port,
    std::string_view remote_host, uint16_t remote_port)
    : protocol_base{flags{}}
    , m_ctx{std::move(ctx)}
    , m_id{*this}
    , m_localHost{local_host}
    , m_remoteHost{remote_host}
    , m_localPort{local_port}
    , m_remotePort{remote_port}
{
  m_impl = new impl{
      {m_localHost, m_localPort, m_ctx->context},
      {m_remoteHost, m_remotePort, m_ctx->context}
  };

  osc_protocol_common::init(*this);
}

osc_udp_protocol::~osc_udp_protocol()
{
  delete m_impl;
}

bool osc_udp_protocol::learning() const
{
  return m_learning;
}

osc_udp_protocol& osc_udp_protocol::set_learning(bool newLearn)
{
  m_learning = newLearn;
  return *this;
}

bool osc_udp_protocol::update(ossia::net::node_base& node)
{
  return false;
}

bool osc_udp_protocol::pull(ossia::net::parameter_base& address)
{
  return false;
}

bool osc_udp_protocol::echo_incoming_message(
    const message_origin_identifier& id, const parameter_base& addr, const value& val)
{
  return osc_protocol_common::echo_incoming_message(*this, id, addr, val);
}

bool osc_udp_protocol::observe(ossia::net::parameter_base& address, bool enable)
{
  return osc_protocol_common::observe(*this, address, enable);
}

void osc_udp_protocol::on_received_message(
    const oscpack::ReceivedMessage& m)
{
  return osc_protocol_common::on_received_message(*this, m);
}

void osc_udp_protocol::set_device(device_base& dev)
{
  m_device = &dev;
}

// Server implementation
bool osc_udp_server::push(const ossia::net::parameter_base& addr, const ossia::value& v)
{
  return osc_protocol_common::push(*this, addr, v);
}

bool osc_udp_server::push(const ossia::net::parameter_base& addr, ossia::value&& v)
{
  return osc_protocol_common::push(*this, addr, std::move(v));
}

bool osc_udp_server::push_raw(const ossia::net::full_parameter_data& addr)
{
  return osc_protocol_common::push_raw(*this, addr);
}

bool osc_udp_server::push_bundle(
    const std::vector<const parameter_base*>& addresses)
{
  return osc_protocol_server::push_bundle(*this, *m_impl, addresses);
}

bool osc_udp_server::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses)
{
  return osc_protocol_server::push_raw_bundle(*this, *m_impl, addresses);
}

// Client implementation
bool osc_udp_client::push(const ossia::net::parameter_base& addr, const ossia::value& v)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  return osc_protocol_common::push(*this, addr, v);
}

bool osc_udp_client::push(const ossia::net::parameter_base& addr, ossia::value&& v)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  return osc_protocol_common::push(*this, addr, std::move(v));
}

bool osc_udp_client::push_raw(const ossia::net::full_parameter_data& addr)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  return osc_protocol_common::push_raw(*this, addr);
}

bool osc_udp_client::push_bundle(
    const std::vector<const parameter_base*>& addresses)
{
  return osc_protocol_client::push_bundle(*this, *m_impl, addresses);
}

bool osc_udp_client::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses)
{
  return osc_protocol_client::push_raw_bundle(*this, *m_impl, addresses);
}

}
