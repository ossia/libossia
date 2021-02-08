// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ossia/network/osc/osc_unix.hpp"

#include <ossia/detail/logger.hpp>
#include <ossia/detail/lockfree_queue.hpp>
#include <ossia/network/context.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/osc/detail/osc_protocol_common.hpp>
#include <ossia/network/value/value.hpp>

#include <asio/local/connect_pair.hpp>

namespace ossia::net
{
struct osc_unix_protocol::impl
{
  unix_socket from_client;
  unix_socket to_client;
};

osc_unix_protocol::osc_unix_protocol(
      network_context_ptr ctx
    , std::string_view socket_name)
  : protocol_base{flags{}}
  , m_ctx{std::move(ctx)}
  , m_id{*this}

{
  m_localSocket = fmt::format("/tmp/{}.server.socket", socket_name);
  m_remoteSocket = fmt::format("/tmp/{}.client.socket", socket_name);
}

osc_unix_protocol::~osc_unix_protocol()
{
  delete m_impl;
}

bool osc_unix_protocol::learning() const
{
  return m_learning;
}

osc_unix_protocol& osc_unix_protocol::set_learning(bool newLearn)
{
  m_learning = newLearn;
  return *this;
}

bool osc_unix_protocol::update(ossia::net::node_base& node)
{
  return false;
}

bool osc_unix_protocol::pull(ossia::net::parameter_base& address)
{
  return false;
}

bool osc_unix_protocol::echo_incoming_message(
    const message_origin_identifier& id, const parameter_base& addr, const value& val)
{
  return osc_protocol_common::echo_incoming_message(*this, id, addr, val);
}

bool osc_unix_protocol::observe(ossia::net::parameter_base& address, bool enable)
{
  return osc_protocol_common::observe(*this, address, enable);
}

void osc_unix_protocol::on_received_message(
    const oscpack::ReceivedMessage& m)
{
  return osc_protocol_common::on_received_message(*this, m);
}

void osc_unix_protocol::set_device(device_base& dev)
{
  m_device = &dev;
}



// Server implementation
osc_unix_server::osc_unix_server(
    network_context_ptr ctx
    , std::string_view socket_name)
  : osc_unix_protocol{std::move(ctx), socket_name}
{
  m_impl = new impl{
      .from_client{m_localSocket, m_ctx->context},
      .to_client{m_remoteSocket, m_ctx->context}
  };
  osc_protocol_common::init(*this);
}

bool osc_unix_server::push(const ossia::net::parameter_base& addr, const ossia::value& v)
{
  return osc_protocol_server::push(*this, addr, v);
}

bool osc_unix_server::push(const ossia::net::parameter_base& addr, ossia::value&& v)
{
  return osc_protocol_server::push(*this, addr, std::move(v));
}

bool osc_unix_server::push_raw(const ossia::net::full_parameter_data& addr)
{
  return osc_protocol_server::push_raw(*this, addr);
}

bool osc_unix_server::push_bundle(
    const std::vector<const parameter_base*>& addresses)
{
  return osc_protocol_server::push_bundle(*this, *m_impl, addresses);
}

bool osc_unix_server::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses)
{
  return osc_protocol_server::push_raw_bundle(*this, *m_impl, addresses);
}

// Client implementation
osc_unix_client::osc_unix_client(
    network_context_ptr ctx
    , std::string_view socket_name)
  : osc_unix_protocol{std::move(ctx), socket_name}
{
  m_impl = new impl{
      .from_client{m_remoteSocket, m_ctx->context},
      .to_client{m_localSocket, m_ctx->context}
  };
  osc_protocol_common::init(*this);
}


bool osc_unix_client::push(const ossia::net::parameter_base& addr, const ossia::value& v)
{
  return osc_protocol_client::push(*this, addr, v);
}

bool osc_unix_client::push(const ossia::net::parameter_base& addr, ossia::value&& v)
{
  return osc_protocol_client::push(*this, addr, std::move(v));
}

bool osc_unix_client::push_raw(const ossia::net::full_parameter_data& addr)
{
  return osc_protocol_client::push_raw(*this, addr);
}

bool osc_unix_client::push_bundle(
    const std::vector<const parameter_base*>& addresses)
{
  return osc_protocol_client::push_bundle(*this, *m_impl, addresses);
}

bool osc_unix_client::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses)
{
  return osc_protocol_client::push_raw_bundle(*this, *m_impl, addresses);
}

}
