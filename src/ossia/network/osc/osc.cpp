// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/algorithms.hpp>
#include <ossia/detail/logger.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/osc/detail/bundle.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/osc_messages.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/osc/osc.hpp>
#include <ossia/network/value/value.hpp>

#include <boost/algorithm/string.hpp>

#include <oscpack/osc/OscPrintReceivedElements.h>
namespace ossia::net
{
using sender_t = osc::sender<osc_1_0_outbound_stream_visitor>;

osc_protocol::osc_protocol(
    std::string ip, uint16_t remote_port, uint16_t local_port,
    std::optional<std::string> expose)
    : protocol_base{flags{SupportsMultiplex}}
    , m_ip{std::move(ip)}
    , m_remote_port{remote_port}
    , m_local_port{local_port}
    , m_expose{std::move(expose)}
    , m_id{*this, {}}
{
  update_sender();
  update_receiver();
  update_zeroconf();
  m_buffer.reserve(128);
}

osc_protocol::~osc_protocol() = default;

const std::string& osc_protocol::get_ip() const
{
  return m_ip;
}

osc_protocol& osc_protocol::set_ip(std::string ip)
{
  m_ip = std::move(ip);
  update_sender();
  update_zeroconf();

  return *this;
}

uint16_t osc_protocol::get_remote_port() const
{
  return m_remote_port;
}

osc_protocol& osc_protocol::set_remote_port(uint16_t in_port)
{
  m_remote_port = in_port;
  update_sender();
  update_zeroconf();

  return *this;
}

uint16_t osc_protocol::get_local_port() const
{
  return m_local_port;
}

osc_protocol& osc_protocol::set_local_port(uint16_t out_port)
{
  m_local_port = out_port;
  update_receiver();
  update_zeroconf();

  return *this;
}

void osc_protocol::update_sender()
{
  m_sender = std::make_unique<sender_t>(m_logger, m_ip, m_remote_port);
  m_id.identifier = (uintptr_t)m_sender.get();
}

void osc_protocol::update_receiver()
{
  if(m_local_port == 0)
  {
    m_receiver.reset();
    return;
  }
  m_receiver = std::make_unique<osc::receiver>(
      m_local_port,
      [this](const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip) {
    this->on_received_message(m, ip);
      });

  if(m_receiver->port() != m_local_port)
  {
    throw ossia::connection_error{
        "osc_protocol: "
        "Could not open port: "
        + std::to_string(m_local_port)};
  }

  m_receiver->run();
}

void osc_protocol::update_zeroconf()
{
  if(!m_expose || m_local_port == 0)
  {
    m_zeroconfServer = {};
    return;
  }
  try
  {
    m_zeroconfServer = net::make_zeroconf_server(
        *m_expose, "_osc._udp", *m_expose, m_local_port, m_remote_port);
  }
  catch(const std::exception& e)
  {
    logger().error("osc_protocol::update_zeroconf: {}", e.what());
  }
  catch(...)
  {
    logger().error("osc_protocol::update_zeroconf: error.");
  }
}

bool osc_protocol::learning() const
{
  return m_learning;
}

osc_protocol& osc_protocol::set_learning(bool newLearn)
{
  m_learning = newLearn;
  return *this;
}

bool osc_protocol::update(ossia::net::node_base& node)
{
  return false;
}

bool osc_protocol::pull(ossia::net::parameter_base& address)
{
  return false;
}

bool osc_protocol::push(const ossia::net::parameter_base& addr, const ossia::value& v)
{
  if(addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = bound_value(addr, v);
  if(val.valid())
  {
    if(m_buffering)
    {
      std::string address = ossia::net::osc_address(addr);
      std::lock_guard lock(m_buffer_mutex);
      m_buffer.emplace_back(address, val);
    }
    else
    {
      m_sender->send(addr, val);
    }
    return true;
  }
  return false;
}

bool osc_protocol::push_raw(const ossia::net::full_parameter_data& addr)
{
  if(addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = bound_value(addr, addr.value());
  if(val.valid())
  {
    if(m_buffering)
    {
      std::lock_guard lock(m_buffer_mutex);
      m_buffer.emplace_back(addr.address, val);
    }
    else
    {
      m_sender->send(addr, val);
    }
    return true;
  }
  return false;
}

bool osc_protocol::push_bundle(const std::vector<const parameter_base*>& addresses)
{
  if(auto bundle = make_bundle(bundle_server_policy<osc_1_0_policy>{}, addresses))
  {
    m_sender->socket().Send(bundle->data.data(), bundle->data.size());
    ossia::buffer_pool::instance().release(std::move(bundle->data));
    return true;
  }
  return false;
}

void osc_protocol::send_buffer()
{
  std::lock_guard lock(m_buffer_mutex);
  push_raw_bundle(m_buffer);
  m_buffer.clear();
}

bool osc_protocol::push_raw_bundle(
    const std::vector<ossia::net::full_parameter_data>& addresses)
{
  if(auto bundle = make_bundle(bundle_server_policy<osc_1_0_policy>{}, addresses))
  {
    m_sender->socket().Send(bundle->data.data(), bundle->data.size());
    ossia::buffer_pool::instance().release(std::move(bundle->data));
    return true;
  }
  return false;
}

bool osc_protocol::observe(ossia::net::parameter_base& address, bool enable)
{
  if(enable)
    m_listening.insert(std::make_pair(address.get_node().osc_address(), &address));
  else
    m_listening.erase(address.get_node().osc_address());

  return true;
}

bool osc_protocol::echo_incoming_message(
    const message_origin_identifier& id, const parameter_base& addr, const value& val)
{
  if(&id.protocol == this && id.identifier == (uintptr_t)(m_sender.get()))
    return true;

  m_sender->send(addr, val);
  return true;
}

void osc_protocol::on_received_message(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
  [[unlikely]] if(!m_device)
  {
    return;
  }

  [[unlikely]] if(m_learning)
  {
    auto already_learned = ossia::net::osc_learn(&m_device->get_root_node(), m);
    if(!already_learned)
      return;
  }

  ossia::net::on_input_message<false>(
      m.AddressPattern(), ossia::net::osc_message_applier{m_id, m}, m_listening,
      *m_device, m_logger);
}

void osc_protocol::set_device(device_base& dev)
{
  m_device = &dev;
}
}
