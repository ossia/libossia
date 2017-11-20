// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <ossia/detail/logger.hpp>
#include <ossia/network/value/value.hpp>
#include <ossia/network/base/parameter.hpp>
#include <ossia/network/domain/domain.hpp>
#include <ossia/network/exceptions.hpp>
#include <ossia/network/generic/generic_parameter.hpp>
#include <ossia/network/generic/generic_device.hpp>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/receiver.hpp>
#include <ossia/network/osc/detail/sender.hpp>
#include <ossia/network/osc/osc.hpp>
#include <boost/algorithm/string.hpp>
#include <oscpack/osc/OscPrintReceivedElements.h>
#include <ossia/network/osc/detail/osc.hpp>
#include <ossia/network/osc/detail/osc_receive.hpp>
namespace ossia
{
namespace net
{
using sender_t = osc::sender<osc_outbound_visitor>;

osc_protocol::osc_protocol(
    std::string ip, uint16_t remote_port, uint16_t local_port, ossia::optional<std::string> expose)
  : m_ip{std::move(ip)}
  , m_remote_port{remote_port}
  , m_local_port{local_port}
  , m_expose{std::move(expose)}
{
  update_sender();
  update_receiver();
  update_zeroconf();
}

osc_protocol::~osc_protocol()
{
}

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
}

void osc_protocol::update_receiver()
{
  m_receiver = std::make_unique<osc::receiver>(
                 m_local_port, [=] (
                    const oscpack::ReceivedMessage& m,
                    const oscpack::IpEndpointName& ip) {
    this->on_received_message(m, ip);
  });

  if (m_receiver->port() != m_local_port)
  {
    throw ossia::connection_error{
      "osc_protocol: "
      "Could not open port: "
      + boost::lexical_cast<std::string>(m_local_port)};
  }

  m_receiver->run();
}

void osc_protocol::update_zeroconf()
{
  if(!m_expose)
    return;
  try
  {
    m_zeroconfServer = net::make_zeroconf_server(
                         *m_expose,
                         "_osc._udp",
                         *m_expose,
                         m_local_port,
                         m_remote_port);
  }
  catch (const std::exception& e)
  {
    logger().error("osc_protocol::update_zeroconf: {}", e.what());
  }
  catch (...)
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

bool osc_protocol::push(const ossia::net::parameter_base& addr)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr);
  if (val.valid())
  {
    m_sender->send(addr, val);
    return true;
  }
  return false;
}

bool osc_protocol::push_raw(const ossia::net::full_parameter_data& addr)
{
  if (addr.get_access() == ossia::access_mode::GET)
    return false;

  auto val = filter_value(addr);
  if (val.valid())
  {
    m_sender->send(addr, val);
    return true;
  }
  return false;
}

bool osc_protocol::push_bundle(const std::vector<const parameter_base*>& addresses)
{
  int N = 1024*1024;

  try {
    auto buffer{std::make_unique<char[]>(N)};
    oscpack::OutboundPacketStream str(buffer.get(), N);
    str << oscpack::BeginBundleImmediate();
    for(auto a : addresses)
    {
      const ossia::net::parameter_base& addr = *a;
      if (addr.get_access() == ossia::access_mode::GET)
        continue;

      ossia::value val = filter_value(addr);
      if (val.valid())
      {
        str << oscpack::BeginMessageN(ossia::net::osc_parameter_string(addr));
        val.apply(osc_outbound_visitor{str});
        str << oscpack::EndMessage();
      }
    }
    str << oscpack::EndBundle();
    m_sender->socket().Send(str.Data(), str.Size());
  }
  catch (const oscpack::OutOfBufferMemoryException&) {
    return false;
  }

  return true;
}

bool osc_protocol::push_raw_bundle(const std::vector<ossia::net::full_parameter_data>& addresses)
{
  int N = 1024*1024;

  try {
    auto buffer{std::make_unique<char[]>(N)};
    oscpack::OutboundPacketStream str(buffer.get(), N);
    str << oscpack::BeginBundleImmediate();
    for(const auto& addr : addresses)
    {
      if (addr.get_access() == ossia::access_mode::GET)
        continue;

      ossia::value val = filter_value(addr);
      if (val.valid())
      {
        str << oscpack::BeginMessageN(ossia::net::osc_parameter_string(addr));
        val.apply(osc_outbound_visitor{str});
        str << oscpack::EndMessage();
      }
    }
    str << oscpack::EndBundle();
    m_sender->socket().Send(str.Data(), str.Size());
  }
  catch (const oscpack::OutOfBufferMemoryException&) {
    return false;
  }

  return true;
}

bool osc_protocol::observe(ossia::net::parameter_base& address, bool enable)
{
  if (enable)
    m_listening.insert(std::make_pair(osc_parameter_string(address), &address));
  else
    m_listening.erase(osc_parameter_string(address));

  return true;
}

void osc_protocol::on_received_message(
    const oscpack::ReceivedMessage& m, const oscpack::IpEndpointName& ip)
{
  if (!m_learning)
  {
    handle_osc_message<false>(m, m_listening, *m_device);
  }
  else
  {
    on_learn(m);
  }

  if (m_logger.inbound_logger)
    m_logger.inbound_logger->info("In: {0}", m);
}

template <std::size_t N>
static bool is_vec(std::vector<ossia::value>& t)
{
  return t.size() == N && ossia::all_of(t, [](const ossia::value& val) {
    return val.getType() == ossia::val_type::FLOAT;
  });
}

void osc_protocol::on_learn(const oscpack::ReceivedMessage& m)
{
  // TODO put them in a hash map instead.
  // Find-or-add algorithm
  ossia::string_view addr = m.AddressPattern();
  // TODO string -> string_view
  std::vector<std::string> v = address_parts(addr);

  bool is_new = false;
  node_base* n = &m_device->get_root_node();
  for (const auto& part : v)
  {
    auto cld = n->find_child(part);
    if (cld)
    {
      n = cld;
    }
    else
    {
      // Start adding
      n = n->create_child(part);
      is_new = true;
    }
  }

  if (!is_new)
    return;

  // Set-up address
  switch (m.ArgumentCount())
  {
    case 0:
    {
      n->create_parameter();
      break;
    }
    case 1:
    {
      auto val = osc_utilities::create_value(m.ArgumentsBegin());
      auto addr = n->create_parameter(val.getType());
      addr->set_value(val);
      break;
    }
    case 2:
    {
      auto val
          = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentCount());
      if (is_vec<2>(val))
      {
        auto addr = n->create_parameter(ossia::val_type::VEC2F);
        addr->set_value(convert<ossia::vec2f>(val));
      }
      else
      {
        auto addr = n->create_parameter(ossia::val_type::LIST);
        addr->set_value(osc_utilities::create_list(
                          m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    case 3:
    {
      auto val
          = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentCount());
      if (is_vec<3>(val))
      {
        auto addr = n->create_parameter(ossia::val_type::VEC3F);
        addr->set_value(convert<ossia::vec3f>(val));
      }
      else
      {
        auto addr = n->create_parameter(ossia::val_type::LIST);
        addr->set_value(osc_utilities::create_list(
                          m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    case 4:
    {
      auto val
          = osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentCount());
      if (is_vec<4>(val))
      {
        auto addr = n->create_parameter(ossia::val_type::VEC4F);
        addr->set_value(convert<ossia::vec4f>(val));
      }
      else
      {
        auto addr = n->create_parameter(ossia::val_type::LIST);
        addr->set_value(osc_utilities::create_list(
                          m.ArgumentsBegin(), m.ArgumentCount()));
      }
      break;
    }
    default:
    {
      auto addr = n->create_parameter(ossia::val_type::LIST);
      addr->set_value(
            osc_utilities::create_list(m.ArgumentsBegin(), m.ArgumentCount()));
      break;
    }
  }
}

void osc_protocol::set_device(device_base& dev)
{
  m_device = &dev;
}
}
}
