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
#include <ossia/network/osc/detail/osc_value_write_visitor.hpp>
#include <ossia/network/osc/detail/bidir.hpp>
#include <ossia/network/osc/detail/message_generator.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <ossia/network/value/value.hpp>

namespace ossia
{
namespace net
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
    : m_localHost{local_host}
    , m_remoteHost{remote_host}
    , m_localPort{local_port}
    , m_remotePort{remote_port}
{
  m_impl = new impl{
      .from_client{m_localHost, m_localPort, ctx->context},
      .to_client{m_remoteHost, m_remotePort, ctx->context}
  };

  m_impl->from_client.open();
  m_impl->to_client.connect();

  m_impl->from_client.receive([this] (const char* data, std::size_t sz)
    {
      auto on_message = [this] (auto&& msg) { this->on_received_message(msg); };
      osc_packet_processor<decltype(on_message)>{on_message}({data, sz});
    }
  );
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

bool osc_udp_protocol::push(const ossia::net::parameter_base& addr, const ossia::value& v)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr, v);
  if (val.valid())
  {
    val.apply(osc_value_send_visitor<udp_socket>{addr.get_node().osc_address(), m_impl->to_client});
    return true;
  }
  return false;
}

bool osc_udp_protocol::push_raw(const ossia::net::full_parameter_data& addr)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr, addr.value());
  if (val.valid())
  {
    val.apply(osc_value_send_visitor<udp_socket>{addr.address, m_impl->to_client});
    return true;
  }
  return false;
}

bool osc_udp_protocol::push_bundle(
    const std::vector<const parameter_base*>& addresses)
{
  if(auto data = make_bundle(addresses)) {
    m_impl->to_client.send(data->stream.Data(), data->stream.Size());
    return true;
  }
  return false;
}

bool osc_udp_protocol::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses)
{
  if(auto data = make_raw_bundle(addresses)) {
    m_impl->to_client.send(data->stream.Data(), data->stream.Size());
    return true;
  }
  return false;
}

bool osc_udp_protocol::observe(ossia::net::parameter_base& address, bool enable)
{
  if (enable)
    m_listening.insert(
        std::make_pair(address.get_node().osc_address(), &address));
  else
    m_listening.erase(address.get_node().osc_address());

  return true;
}

void osc_udp_protocol::on_received_message(
    const oscpack::ReceivedMessage& m)
{
  if (!m_learning)
  {
    handle_osc_message<false>(m, m_listening, *m_device, m_logger);
  }
  else
  {
    ossia::net::osc_learn(&m_device->get_root_node(), m);
  }
}


void osc_udp_protocol::set_device(device_base& dev)
{
  m_device = &dev;
}
}
}
